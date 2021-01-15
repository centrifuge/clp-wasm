/*
This file is part of C++lex, a project by Tommaso Urli.

C++lex is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

C++lex is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with C++lex.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "simplex.h"
#include "matrix.h"
#include "variable.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <streambuf>

#ifndef __EMSCRIPTEN__
#include <filesystem>
namespace fs = std::filesystem;

std::string resolvePath(const std::string & relPath)
{
    auto baseDir = fs::current_path();
    while (baseDir.has_parent_path())
    {
        auto combinePath = baseDir / relPath;
        if (exists(combinePath))
        {
            return combinePath.string();
        }
        if (baseDir == baseDir.parent_path())
            break;
        baseDir = baseDir.parent_path();
    }
    return {};
}
#endif

const float_type TOL("0.0000000000000000000000000001");
const float_type ZERO = 0.0;
const float_type ONE = 1.0;

#define VERBOSE 0

using pilal::tol_equal;
using std::ifstream;
using std::stod;
using std::string;
using std::stringstream;
using std::vector;

struct Membuf : std::streambuf
{
    Membuf(char const * base, const size_t size)
    {
        char * p(const_cast<char *>(base));
        this->setg(p, p, p + size);
    }
    ~Membuf() override = default;
};
struct Imemstream : virtual Membuf, std::istream
{
    Imemstream(char const * base, const size_t size)
    : Membuf(base, size)
    , std::istream(static_cast<std::streambuf *>(this))
    {
    }
};

namespace optimization
{

// Enum to individuate parsing context
enum ParsingContext
{
    PB_METADATA,
    PB_VARS,
    PB_CONSTRAINTS,
    PB_OBJECTIVE
};

/*
    Simplex
    =======
    Implementation of the class that will incapsulate the
    simplex behavior.
*/

Simplex::Simplex(std::string name)
: name(std::move(name))
, solution_dimension(0)
, changed_sign(false)
, inverse_recalculation_rate(10)
{
}

Simplex::~Simplex()
{
    // Cleanup variables
    std::vector<Variable *>::iterator it;
    for (it = variables.begin(); it != variables.end(); ++it)
        if ((*it)->creator == this)
            delete *it;
}

void Simplex::add_variable(Variable * variable)
{
    variables.push_back(variable);
}

bool Simplex::has_solutions() const
{
    return !overconstrained;
}

bool Simplex::is_unlimited() const
{
    return unlimited;
}

bool Simplex::must_be_fixed() const
{
    return has_to_be_fixed;
}

Matrix const & Simplex::get_dual_variables() const
{
    return dual_variables;
}

void Simplex::load_lp_problem(std::istream & is)
{
    using namespace std;
    /*
Maximize
x + y + z
Subject To
c0: x + y = 1
c1: x + 5 y + 2 z <= 10
qc0: x + y + [ x ^ 2 - 2 x * y + 3 y ^ 2 ] <= 5
Bounds
0 <= x <= 5
z >= 2
Generals
x y z
End
    */
    string content { istreambuf_iterator<char>(is), istreambuf_iterator<char>() };

    static const std::vector<string> SECTIONS = { "Maximize", "Minimize", "Subject To", "Bounds", "Generals", "End" };
}

void Simplex::load_problem(const std::string & problem_input)
{
    using namespace std;
    // auto path = resolvePath(problem_name);

    auto problem_content = problem_input;
    ifstream file(problem_input.c_str());
    if (file.good())
    {
        problem_content = string { istreambuf_iterator<char>(file), istreambuf_iterator<char>() };
    }
    Imemstream stream(reinterpret_cast<char *>(&problem_content[0]), problem_content.size());

    ParsingContext current_parsing_block;
    int current_var = 0, solution_dimension = 0;

    while (stream.good())
    {
        string buffer_init, token;
        getline(stream, buffer_init);
        stringstream buffer(buffer_init);

        // Extract token
        buffer >> token;

        if (token.length())
        {
            if (token == "[METADATA]")
                current_parsing_block = PB_METADATA;
            else if (token == "[VARIABLES]")
                current_parsing_block = PB_VARS;
            else if (token == "[CONSTRAINTS]")
                current_parsing_block = PB_CONSTRAINTS;
            else if (token == "[OBJECTIVE]")
                current_parsing_block = PB_OBJECTIVE;
            else
            {
                switch (current_parsing_block)
                {

                    case PB_METADATA:
                    {
                        if (token == "name")
                        {
                            buffer_init.erase(0, 5);
                            name = buffer_init;
                        }
                        else if (token == "vars")
                        {
                            buffer >> solution_dimension;
                        }
                    }
                    break;

                    case PB_VARS:
                    {
                        Matrix eye(1, solution_dimension, 0);
                        eye(current_var) = 1;
                        string variable_name, lower_bound, upper_bound;

                        lower_bound = token;
                        buffer >> variable_name;
                        buffer >> upper_bound;

                        // Add variable for tracking
                        add_variable(new Variable(this, variable_name.c_str()));

                        if (lower_bound != "inf")
                        {
                            if (stof(lower_bound) == 0)
                                add_constraint(Constraint(eye, CT_NON_NEGATIVE, 0));
                            else
                                add_constraint(Constraint(eye, CT_MORE_EQUAL, float_type(lower_bound)));
                        }
                        if (upper_bound != "inf")
                            add_constraint(Constraint(eye, CT_LESS_EQUAL, float_type(upper_bound)));

                        current_var++;
                    }
                    break;

                    case PB_CONSTRAINTS:
                    {
                        if (current_var != solution_dimension)
                            throw std::runtime_error("Mismatch between declared and defined variables.");

                        Matrix coefficients(1, solution_dimension);
                        coefficients(0) = float_type(token);

                        for (int i = 1; i < solution_dimension; ++i)
                            buffer >> coefficients(i);

                        string ct, bound;
                        buffer >> ct;
                        buffer >> bound;

                        if (ct == ">" || ct == ">=")
                            add_constraint(Constraint(coefficients, CT_MORE_EQUAL, float_type(bound)));
                        else if (ct == "<" || ct == "<=")
                            add_constraint(Constraint(coefficients, CT_LESS_EQUAL, float_type(bound)));
                        else if (ct == "=")
                            add_constraint(Constraint(coefficients, CT_EQUAL, float_type(bound)));
                        else
                        {
                            stringstream parse_error;
                            parse_error << " near ";
                            parse_error << buffer_init.c_str();
                            throw std::runtime_error("Data mismatch error: " + parse_error.str());
                        }
                    }
                    break;

                    case PB_OBJECTIVE:
                    {
                        string oft = token;
                        Matrix costs(1, solution_dimension);
                        for (int i = 0; i < solution_dimension; ++i)
                        {
                            std::string cost;
                            buffer >> cost;
                            costs(i) = float_type(cost);
                        }

                        if (oft == "maximize")
                            set_objective_function(ObjectiveFunction(OFT_MAXIMIZE, costs));
                        else if (oft == "minimize")
                            set_objective_function(ObjectiveFunction(OFT_MINIMIZE, costs));
                        else
                            throw std::runtime_error("Data mismatch error: Unknown objective function kind.");
                    }
                    break;
                }
            }
        }
    }
}

void Simplex::add_constraint(const Constraint & constraint)
{

    if (constraints.size() != 0)
    {
        if (solution_dimension != constraint.coefficients.dim().second)
            throw(DataMismatchException("Constraints must have the same size"));
    }
    else
    {
        solution_dimension = constraint.size();
    }

    if (constraint.type == CT_NON_NEGATIVE)
    {
        nn_constraints.push_back(constraint);
    }
    else
    {
        constraints.push_back(constraint);
    }
}

void Simplex::set_objective_function(ObjectiveFunction const & objective_function)
{

    if (solution_dimension != objective_function.costs.dim().second)
        throw(DataMismatchException("Objective function must have same size as solution"));

    this->objective_function = objective_function;
}

void Simplex::log() const
{
    // Title
    for (unsigned int i = 0; i < name.length(); ++i)
        std::cout << "=";
    std::cout << std::endl;
    std::cout << name << std::endl;
    for (unsigned int i = 0; i < name.length(); ++i)
        std::cout << "=";
    std::cout << std::endl;

    // Constraints
    vector<Constraint>::const_iterator it;

    // Regular
    std::cout << std::endl;
    std::cout << "Number of regular constraints: " << constraints.size() << std::endl;
    for (it = constraints.begin(); it != constraints.end(); ++it)
        it->log();

    // Non negativity
    std::cout << std::endl;
    std::cout << "Number of non-negativity constraints: " << nn_constraints.size() << std::endl;
    for (it = nn_constraints.begin(); it != nn_constraints.end(); ++it)
        it->log();

    std::cout << std::endl;
    std::cout << "Objective function:" << std::endl;
    objective_function.log();
    std::cout << std::endl;
}

void Simplex::process_to_standard_form()
{

    // Constraint iterator
    vector<Constraint>::iterator it;

    // Process non-negative constraints
    int initial_solution_dimension = solution_dimension;
    for (int i = 0; i < initial_solution_dimension; ++i)
    { // For each component of x

        bool has_constraint = false;

        // Find an x that doesn't have a non-negativity constraint on it
        for (it = nn_constraints.begin(); it != nn_constraints.end() && !has_constraint; ++it)
            if (it->coefficients(i) == 1)
                has_constraint = true;

        if (!has_constraint)
        {

            // Add a non-negativity constraint
            Matrix eye(1, solution_dimension);
            eye(i) = 1.0;
            this->add_constraint(Constraint(eye, CT_NON_NEGATIVE, 0.0));

            ++solution_dimension;

            // Add a column to all constraints
            vector<Constraint>::iterator mit;
            for (mit = nn_constraints.begin(); mit != nn_constraints.end(); ++mit)
                mit->add_column(0.0);

            // Add another non-negativity constraint
            Matrix n_eye(1, solution_dimension);
            n_eye(solution_dimension - 1) = 1.0;

            this->add_constraint(Constraint(n_eye, CT_NON_NEGATIVE, 0.0));

            // Add a regular constraint
            for (mit = constraints.begin(); mit != constraints.end(); ++mit)
                mit->add_column(-mit->coefficients(i));

            objective_function.add_column(-objective_function.costs(i));

            // Update variables status
            string aux_name(variables.at(i)->name);
            Variable * auxiliary = new AuxiliaryVariable(this,
                                                         (aux_name + "_minus").c_str(),
                                                         static_cast<int>(variables.size()));
            Variable * splitted = new SplittedVariable(this,
                                                       variables.at(i)->name.c_str(),
                                                       (AuxiliaryVariable *)auxiliary);

            // Modify variables
            variables.at(i) = splitted;
            variables.push_back(auxiliary);
        }
    }

    // Process regular constraints
    for (it = constraints.begin(); it != constraints.end(); ++it)
    {
        if (it->type == CT_MORE_EQUAL)
        {
            vector<Constraint>::iterator mit;

            // Add empty column to all regular constraints except the current
            for (mit = constraints.begin(); mit != constraints.end(); ++mit)
                if (mit != it)
                    mit->add_column(0);

            for (mit = nn_constraints.begin(); mit != nn_constraints.end(); ++mit)
                mit->add_column(0);

            // Add a 1 column to the current
            it->add_column(-1);
            it->type = CT_EQUAL;
            objective_function.add_column(0);
            ++solution_dimension;

            // Add constraint
            Matrix eye(1, solution_dimension);
            eye(solution_dimension - 1) = 1;
            this->add_constraint(Constraint(eye, CT_NON_NEGATIVE, 0));

            // Update variables vector
            stringstream variable_name;
            variable_name << "slack_";
            variable_name << solution_dimension - 1;
            variables.push_back(new SlackVariable(this, variable_name.str().c_str()));
        }
        else if (it->type == CT_LESS_EQUAL)
        {
            vector<Constraint>::iterator mit;

            // Add empty column to all regular constraints except the current
            for (mit = constraints.begin(); mit != constraints.end(); ++mit)
                if (mit != it)
                    mit->add_column(0);

            for (mit = nn_constraints.begin(); mit != nn_constraints.end(); ++mit)
                mit->add_column(0);

            // Add a 1 column to the current
            it->add_column(1);
            it->type = CT_EQUAL;
            objective_function.add_column(0);
            ++solution_dimension;

            // Add constraint
            Matrix eye(1, solution_dimension);
            eye(solution_dimension - 1) = ONE;
            this->add_constraint(Constraint(eye, CT_NON_NEGATIVE, 0));

            // Update variables vector
            stringstream variable_name;
            variable_name << "slack_";
            variable_name << solution_dimension - 1;
            variables.push_back(new SlackVariable(this, variable_name.str().c_str()));
        }
    }

    // Manipulate objective function
    if (objective_function.type == OFT_MAXIMIZE)
    {
        objective_function.type = OFT_MINIMIZE;
        Matrix zero(1, solution_dimension, ZERO);
        changed_sign = true;
        objective_function.costs = zero - objective_function.costs;
    }

    // Update name
    name += " (Standard Form)";
}

void Simplex::process_to_artificial_problem()
{
    ColumnSet identity;

    // Scans all the columns, when I find a column that is an eye for i
    // put it in the base at position i

    for (unsigned int i = 0; i < constraints.size(); ++i)
    {
        if (VERBOSE)
            std::cout << std::endl;
        if (VERBOSE)
            std::cout << "Checking for column " << i << " of identity." << std::endl;

        bool column_not_found = true;

        for (int c = solution_dimension - 1; c > -1 && column_not_found; --c)
        {
            if (VERBOSE)
                std::cout << "Checking against column " << c << std::endl;

            bool column_match = true;

            for (unsigned int j = 0; j < constraints.size() && column_match; ++j)
            {
                column_match = true;

                if ((i == j && (constraints.at(j).coefficients(c) != ONE))
                    || (i != j && constraints.at(j).coefficients(c) != ZERO))
                    column_match = false;
            }

            if (column_match)
            {
                if (VERBOSE)
                    std::cout << "Row match." << std::endl;
                identity.insert(c);
                column_not_found = false;
            }
        }

        if (column_not_found)
        {
            if (VERBOSE)
                std::cout << "Column not found, added artificial variable." << std::endl;
            identity.insert(-1);
        }
        else
        {
            if (VERBOSE)
                std::cout << "Column found and added to identity." << std::endl;
        }
    }

    // If artificial variables are needed
    objective_function.costs.empty();

    if (identity.contains(-1))
    {
        for (unsigned int i = 0; i < identity.size(); ++i)
        {
            if (identity.column(i) == -1)
            {
                // Add column 1 to constraint i
                for (unsigned int k = 0; k < constraints.size(); ++k)
                    if (k == i)
                        constraints.at(k).add_column(1);
                    else
                        constraints.at(k).add_column(0);

                // Solution vector is bigger
                identity.column(i) = solution_dimension;
                ++solution_dimension;

                // Create non-negative constraint for new variable
                Matrix eye(1, solution_dimension);
                eye(solution_dimension - 1) = ONE;

                for (auto k = 0; k < nn_constraints.size(); ++k)
                    nn_constraints.at(k).add_column(ZERO);

                // Objective function costs updated
                objective_function.add_column(1.0);

                this->add_constraint(Constraint(eye, CT_NON_NEGATIVE, ZERO));
            }
        }
    }
    suggested_base = identity;

    name += " --> (Artificial problem)";
}

void Simplex::solve_with_base(ColumnSet const & initial_base)
{
    // Preprocess constraints data to lead to matrices
    coefficients_matrix.resize( // A
        static_cast<int>(constraints.size()),
        solution_dimension);

    constraints_vector.resize(static_cast<int>(constraints.size()), 1);

    for (unsigned int i = 0; i < constraints.size(); ++i)
    {
        // Set b
        constraints_vector(i) = constraints.at(i).value;
        for (int j = 0; j < solution_dimension; ++j)
            coefficients_matrix(i, j) = constraints.at(i).coefficients(j);
    }

    // Copy costs
    costs = objective_function.costs;

    // Initialize algorithm
    int step = 0;

    if (initial_base.size() == constraints.size())
        current_base = initial_base;
    else
        throw(DataMismatchException("Wrong initial base size!"));

    // Exported variables
    optimal = false;
    unlimited = false;

    while (!optimal && !unlimited)
    {
        // Temporary matrices
        Matrix u; // c_b * B^-1
        Matrix base_costs(1, (int)current_base.size()); // Costs of base

        // Populate current_out_of_base
        current_out_of_base.columns.clear();
        for (int i = 0; i < solution_dimension; ++i)
            if (!current_base.contains(i))
                current_out_of_base.insert(i);

        // Every inverse_recalculation steps recompute inverse from scratch
        if (step % inverse_recalculation_rate == 0)
        {
            Matrix base_matrix((int)current_base.size());

            // Unpack current base and objective costs
            for (unsigned int j = 0; j < current_base.size(); ++j)
            {
                base_costs(j) = costs(current_base.column(j));

                for (unsigned int i = 0; i < current_base.size(); ++i)
                    base_matrix(i, j) = coefficients_matrix(i, current_base.column(j));
            }

            // Compute inverse
            base_matrix.get_inverse(base_inverse);
        }
        else
        {
            // Unpack objective costs
            for (unsigned int j = 0; j < current_base.size(); ++j)
            {
                base_costs(j) = costs(current_base.column(j));
            }

            Matrix old_inverse = base_inverse;

            // Compute inverse
            Matrix::get_inverse_with_column(old_inverse, column_p, old_column, base_inverse);
        }

        if (VERBOSE)
            std::cout << "Step: " << step << std::endl;
        ++step;

        if (VERBOSE)
            current_base.log("Columns in base: ");
          if (VERBOSE)
        current_out_of_base.log("Out of base: ");
        if (VERBOSE)
            base_inverse.log("Base inverse is:");

        // Compute x_B = B^-1 * b
        base_solution = base_inverse * constraints_vector;

        // Compute u = c_B * A;
        u = base_costs * base_inverse;

        if (VERBOSE)
            u.log("U");

        // Compute reduced cost
        reduced_cost = costs - (u * coefficients_matrix);

         if (VERBOSE)
        reduced_cost.log("Current reduced cost is");

        optimal = reduced_cost.more_equal_than(0, TOL);

        bool degenerate = false;
        for (unsigned int i = 0; i < current_base.size() && degenerate == false; ++i)
            if (tol_equal(base_solution(i), 0, TOL))
                degenerate = true;

        if (!optimal)
        {
            if (VERBOSE)
                std::cout << "Base not optimal since reduced cost is negative." << std::endl;

            // Column of reduced cost with min value (one of the policies)
            int p = -1;
            column_p.resize(static_cast<int>(constraints.size()), 1);
            Matrix a_tilde;

            // Bland's strategy
            for (unsigned int i = 0; i < current_out_of_base.size() && p == -1; ++i)
                if (reduced_cost(current_out_of_base.column(i)) < ZERO)
                    p = current_out_of_base.column(i);

            if (p == -1)
            {
                std::cout << "\nNo reduced cost was negative" << std::endl;
                p = current_out_of_base.column(0);
                for (unsigned int i = 0; i < current_out_of_base.size(); ++i)
                    std::cout << reduced_cost(current_out_of_base.column(i)) << " ";
            }
            for (unsigned int i = 0; i < constraints.size(); ++i)
                column_p(i) = coefficients_matrix(i, p);

            if (VERBOSE)
                std::cout << "The column to insert is " << p << std::endl;
            if (VERBOSE)
                column_p.log("That is ...");

            // Compute a_tilde
            a_tilde = base_inverse * column_p;

            if (VERBOSE)
                a_tilde.log("a_tilde");

            unlimited = a_tilde.less_equal_than(0, TOL);

            if (!unlimited)
            {
                if (VERBOSE)
                    std::cout << "Problem not unlimited." << std::endl;

                // Bland's strategy
                int q_position = -1;
                for (unsigned int i = 0; i < current_base.size(); ++i)
                {
                    auto value = base_solution(i) / a_tilde(i);
                    if (a_tilde(i) > 0
                        && (q_position == -1 || value < (base_solution(q_position) / a_tilde(q_position))))
                        q_position = i;
                }

                int q = current_base.column(q_position);
                old_column = q_position;

                if (VERBOSE)
                    std::cout << "The column to take off is " << q << std::endl;

                // Take off q, push in p
                current_base.substitute(q, p);
            }
            else
            {
                std::cout << "Problem unlimited." << std::endl;
            }
        }
        else
        {
            std::cout << "Optimal found at step " << step << "." << std::endl;
            Matrix objective_function_base(1, (int)current_base.size(), 0);
            Matrix full_solution(solution_dimension, 1, 0);

            // Update dual variables
            dual_variables = u;

            for (unsigned int i = 0; i < constraints.size(); ++i)
                objective_function_base(i) = costs(current_base.column(i));

            for (int i = 0; i < solution_dimension; ++i)
                if (current_base.contains(i))
                    full_solution(i) = base_solution(current_base.index_of(i));

            if (VERBOSE)
                full_solution.log("Solution:");

            // Saves some flops
            solution_value = (float_type)(objective_function_base * base_solution);

            if (changed_sign)
                solution_value = -solution_value;

            if (VERBOSE)
                std::cout << "Solution value: " << solution_value << std::endl;

            solution = full_solution;
        }
    }
}

std::string Simplex::get_solution() const
{
    stringstream ss;
    ss << std::setprecision(std::numeric_limits<float_type>::max_digits10) << std::boolalpha;

    const auto printKeyVal =
        [&ss](int indentLevel, const std::string & name, const auto & value, const std::string & terminator = ",\n") {
            std::string indent(indentLevel * 4, ' ');
            ss << indent << '"' << name << "\": \"" << value << '"' << terminator;
        };

    ss << "{" << std::endl;
    for (int i = 0; i < solution_dimension; ++i)
        printKeyVal(1, variables.at(i)->name, solution(i));

    auto dual_problem_value = (float_type)(dual_variables * constraints_vector);
    if (changed_sign)
        dual_problem_value *= -1;

    printKeyVal(1, "unlimited", unlimited);
    printKeyVal(1, "overconstrained", overconstrained);
    printKeyVal(1, "solutionCostResult", solution_value);
    printKeyVal(1, "dualProblemValue", dual_problem_value, "\n");

    ss << "}";
    return ss.str();
}

void Simplex::print_solution() const
{
    std::cout << get_solution() << std::endl;
}

void Simplex::solve()
{

    ColumnSet initial_base;

    // Create alias to *this
    Simplex & original_problem = *this;

    // Create problem to work on
    Simplex standard_form_problem = original_problem;

    has_to_be_fixed = false;

    log();

    // Preprocessing
    std::cout << "Generating problem in standard form ...";
    standard_form_problem.process_to_standard_form();
    std::cout << " done." << std::endl;

    if (VERBOSE)
        standard_form_problem.log();

    // Generate and solve artificial problem
    {
        // Create copy of standard form problem to create artificial problem
        Simplex artificial_problem = standard_form_problem;

        std::cout << "Generating artificial problem ...";
        artificial_problem.process_to_artificial_problem();
        std::cout << " done." << std::endl;

        if (VERBOSE)
            artificial_problem.log();

        // Use artificial problem suggested base to solve it
        std::cout << "Solving artificial problem ..." << std::endl;
        artificial_problem.solve_with_base(artificial_problem.suggested_base);
        std::cout << "Done." << std::endl;

        if (!artificial_problem.solution_value.is_zero())
        {
            std::cout << "Problem has no solution." << std::endl;
            overconstrained = true;
            return;
        }
        else
        {
            overconstrained = false;
            if (VERBOSE)
                std::cout << "Suggested initial base for original problem:";
            if (VERBOSE)
                artificial_problem.current_base.log(" ");

            // If initial base doesn't contain artificial variables
            // I can just use it, otherwise it may contain an artificial
            // variable.

            // Check for existence of a column index related  to an artificial
            // variable by reading costs vector
            int artificial_variable = -1;

            for (int i = 0; i < artificial_problem.solution_dimension; ++i)
                if (artificial_problem.objective_function.costs(i) == ONE && artificial_problem.current_base.contains(i))
                    artificial_variable = i;

            // If index is still -1 (no artificial variables)
            if (artificial_variable == -1)
            {

                std::cout << "Base is clear about artificial variables, proceed ..." << std::endl;
                standard_form_problem.suggested_base = artificial_problem.current_base;
            }
            else
            {
                /*
                    If an artificial variable exists ... I can change the i (artificial)
                    column with a j column in current_out_of base so that:

                        *   j is not an auxiliary variable
                        *   (B^-1)_q * A^j != 0
                */

                if (VERBOSE)
                    std::cout << "Artificial variable detected in base: " << artificial_variable << std::endl;
                int q = artificial_problem.current_base.index_of(artificial_variable);
                Matrix bi_row_q(1, (int)artificial_problem.current_base.size());

                for (unsigned int k = 0; k < artificial_problem.current_base.size(); ++k)
                    bi_row_q(k) = artificial_problem.base_inverse(q, k);

                // Find j
                int j = -1;
                for (unsigned int i = 0; i < standard_form_problem.current_out_of_base.size() && j == -1; ++i)
                {
                    // Pick the ones that doesn't refer to an artificial variable
                    if (artificial_problem.costs(i).is_zero())
                    {
                        Matrix column_j((int)standard_form_problem.current_base.size(), 1);
                        for (unsigned int k = 0; k < standard_form_problem.current_base.size(); ++k)
                            column_j(k) = artificial_problem.coefficients_matrix(k, i);
                        if (!((float_type)(bi_row_q * column_j)).is_zero())
                            j = i;
                    }
                }

                if (j != -1)
                {
                    // Found a j, substitute artificial_value with j
                    standard_form_problem.suggested_base = artificial_problem.current_base;
                    standard_form_problem.suggested_base.substitute(artificial_variable, j);
                    if (VERBOSE)
                        standard_form_problem.suggested_base.log("Now initial base is");
                }
                else
                {
                    /*
                    I didn't find a j which respected the requirements.
                    It may happen that for each j we have (B^-1)_q * A^j = 0,
                    this means that the rows of A are linearly dependent and
                    we can eliminate one of them. Let d be

                        d = e_q * B^-1

                    We have to eliminate a row for which d is non-zero.
                    */

                    std::cout << "Constraints are linearly dependent!" << std::endl;

                    // Find a constraint to eliminate (change)
                    int change = -1;
                    for (unsigned int i = 0; i < standard_form_problem.constraints.size() && change == -1; ++i)
                        if (!bi_row_q(i).is_zero())
                            change = i;

                    std::cout << "Constraint #" << change << " must be eliminated." << std::endl;
                    has_to_be_fixed = true;
                    return;
                }
            }
        }
    }

    std::cout << "Solving problem ..." << std::endl;
    standard_form_problem.solve_with_base(standard_form_problem.suggested_base);
    std::cout << "Done." << std::endl;

    /*
        The solution of the standard form problem must be transposed to
        the original problem.
    */

    std::cout << "Processing standard form solution ..." << std::endl;

    if (standard_form_problem.unlimited)
    {
        unlimited = true;
    }
    else
    {
        unlimited = false;
        solution.resize(solution_dimension, 1);

        vector<Variable *>::const_iterator it;
        int index = 0;
        for (auto & variable : standard_form_problem.variables)
            variable->process(standard_form_problem.solution, solution, index++);

        solution_value = standard_form_problem.solution_value;
        dual_variables = standard_form_problem.dual_variables;
        constraints_vector = standard_form_problem.constraints_vector;
        changed_sign = standard_form_problem.changed_sign;
    }
    std::cout << "Done." << std::endl;
}
} // namespace optimization

# CLP Linear Programming solver ported to WebAssembly

This project brings linear programming library CLP to the browser and node.js via WebAssembly. Given the limited precision of standard double arithmetics when dealing with values over `2^53`, the library is compiled with custom floating point precision mapped to `boost::multiprecision::number<mp::cpp_dec_float<100>, mp::et_off>` (i.e. floating point with 100 decimal values).
The interface supports a high level method `solve(problem: string, precision?: number): string`, where `problem` is a string specifyign the linear programming input in [LP format](https://www.ibm.com/support/knowledgecenter/SSSA5P_12.7.1/ilog.odms.cplex.help/CPLEX/FileFormats/topics/LP.html) and `precision` is just the numeric precision of the values printed as strings when returning the result object. However a low-level API also exist for finer grain control.

## Live demo

You can try the solver [here](https://centrifuge.github.io/clp-wasm/)

## Building from source

You'll need docker installed in your system. Simply run:

```bash
npm install
npm run build
npm run test # Optionally run the test suite
```

## Usage

For a browser-based example, please look at the live demo.

To install from NPM

```bash
npm install clp-wasm
```

Then import the library in Javasript and use it:

```javascript
require("clp-wasm/clp-wasm").then((clp) => {
  const lp = `Maximize
   obj: + 0.6 x1 + 0.5 x2
   Subject To
   cons1: + x1 + 2 x2 <= 1
   cons2: + 3 x1 + x2 <= 2
   End`;
  console.log(clp.solve(lp)); // Prints a result object with solution values, objective, etc.
});
```

The same problem can also be solved with a lower level API:

```javascript
require("clp-wasm/clp-wasm").then((clp) => {
  const wrapper = new clp.ClpWrapper();
  const InfU = +Number.MAX_VALUE;
  const InfL = -Number.MAX_VALUE;
  const obj = [-0.6, -0.5]; // expressed as a minimization problem
  const col_lb = [InfL, InfL]; // variable lower bounds
  const col_up = [InfU, InfU]; // variable upper bounds
  const row_lb = [InfL, InfL]; // row constraints lower bounds
  const row_ub = [1, 2]; // row constraints upper bounds
  const matrix = [1, 2, 3, 1]; // Matrix coefficients
  let success = wrapper.loadProblem(obj, col_lb, col_up, row_lb, row_ub, matrix); // returns true if the problem dimensions matched
  wrapper.primal();
  const solution = wrapper.getSolutionArray(1); // The solution vector
  const infeasibilityRay = wrapper.getInfeasibilityRay(1); // A vector proving infeasability if the problem is over-constrained (empty in this case)
  const unboundedRay = wrapper.getUnboundedRay(1); // A vector proving the problem is unbounded (empty in this case)
});
```

If you don't want to deal with the `clp-wasm.wasm` asset contet and don't mind the extra size and Base64 conversion, `clp-wasm.all.js` includes the wasm blob as a Base64 string can be used without any extra dependencies.

## Diving into the code

`ClpWrapper` is the main class to look at. It wraps an instance of `ClpSimplex`, which is the main class in the Clp library. in `solver/bindings.cc` you can see what methods of the class are exposed to Javascript.

The user guide of Clp itself can be found [here](https://coin-or.github.io/Clp/) and a description of the methods that allows the user to load/set the problem, control algorithm paramters and get at the solution can be found [here](https://coin-or.github.io/Clp/basicmodelclasses). Only some methods are exposed to Javascript at the moment.

Clp code comes with a relative large number of verbose messages that are printed to standard output (console when compiled to webasembly) by default. Because of that, we suppressed when creating the javascript glue code that ultimately executes WebAssembly. If you want/need to see Clp native output, you can rebuild the module after commenting out the line `Module['print'] = (x) => { };` in `common/pre.js` with `npm run build`.

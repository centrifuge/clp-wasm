/* $Id: ClpConstraint.hpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2007, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef ClpConstraint_H
#define ClpConstraint_H

//#############################################################################
class ClpSimplex;
class ClpModel;

/** Constraint Abstract Base Class

Abstract Base Class for describing a constraint or objective function

*/
class ClpConstraint {

public:
  ///@name Stuff
  //@{

  /** Fills gradient.  If Linear then solution may be NULL,
         also returns true value of function and offset so we can use x not deltaX in constraint
         If refresh is false then uses last solution
         Uses model for scaling
         Returns non-zero if gradient undefined at current solution
     */
  virtual int gradient(const ClpSimplex *model,
    const FloatT *solution,
    FloatT *gradient,
    FloatT &functionValue,
    FloatT &offset,
    bool useScaling = false,
    bool refresh = true) const = 0;
  /// Constraint function value
  virtual FloatT functionValue(const ClpSimplex *model,
    const FloatT *solution,
    bool useScaling = false,
    bool refresh = true) const;
  /// Resize constraint
  virtual void resize(int newNumberColumns) = 0;
  /// Delete columns in  constraint
  virtual void deleteSome(int numberToDelete, const int *which) = 0;
  /// Scale constraint
  virtual void reallyScale(const FloatT *columnScale) = 0;
  /** Given a zeroed array sets nonlinear columns to 1.
         Returns number of nonlinear columns
      */
  virtual int markNonlinear(char *which) const = 0;
  /** Given a zeroed array sets possible nonzero coefficients to 1.
         Returns number of nonzeros
      */
  virtual int markNonzero(char *which) const = 0;
  //@}

  ///@name Constructors and destructors
  //@{
  /// Default Constructor
  ClpConstraint();

  /// Copy constructor
  ClpConstraint(const ClpConstraint &);

  /// Assignment operator
  ClpConstraint &operator=(const ClpConstraint &rhs);

  /// Destructor
  virtual ~ClpConstraint();

  /// Clone
  virtual ClpConstraint *clone() const = 0;

  //@}

  ///@name Other
  //@{
  /// Returns type, 0 linear, 1 nonlinear
  inline int type()
  {
    return type_;
  }
  /// Row number (-1 is objective)
  inline int rowNumber() const
  {
    return rowNumber_;
  }

  /// Number of possible coefficients in gradient
  virtual int numberCoefficients() const = 0;

  /// Stored constraint function value
  inline FloatT functionValue() const
  {
    return functionValue_;
  }

  /// Constraint offset
  inline FloatT offset() const
  {
    return offset_;
  }
  /// Say we have new primal solution - so may need to recompute
  virtual void newXValues() {}
  //@}

  //---------------------------------------------------------------------------

protected:
  ///@name Protected member data
  //@{
  /// Gradient at last evaluation
  mutable FloatT *lastGradient_;
  /// Value of non-linear part of constraint
  mutable FloatT functionValue_;
  /// Value of offset for constraint
  mutable FloatT offset_;
  /// Type of constraint - linear is 1
  int type_;
  /// Row number (-1 is objective)
  int rowNumber_;
  //@}
};

#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

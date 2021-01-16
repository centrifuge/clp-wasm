/* $Id: ClpQuadraticObjective.hpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef ClpQuadraticObjective_H
#define ClpQuadraticObjective_H

#include "ClpObjective.hpp"
#include "CoinPackedMatrix.hpp"

//#############################################################################

/** Quadratic Objective Class

*/

class ClpQuadraticObjective : public ClpObjective {

public:
  ///@name Stuff
  //@{

  /** Returns gradient.  If Quadratic then solution may be NULL,
         also returns an offset (to be added to current one)
         If refresh is false then uses last solution
         Uses model for scaling
         includeLinear 0 - no, 1 as is, 2 as feasible
     */
  virtual FloatT *gradient(const ClpSimplex *model,
    const FloatT *solution, FloatT &offset, bool refresh,
    int includeLinear = 2);
  /// Resize objective
  /** Returns reduced gradient.Returns an offset (to be added to current one).
     */
  virtual FloatT reducedGradient(ClpSimplex *model, FloatT *region,
    bool useFeasibleCosts);
  /** Returns step length which gives minimum of objective for
         solution + theta * change vector up to maximum theta.

         arrays are numberColumns+numberRows
         Also sets current objective, predicted and at maximumTheta
     */
  virtual FloatT stepLength(ClpSimplex *model,
    const FloatT *solution,
    const FloatT *change,
    FloatT maximumTheta,
    FloatT &currentObj,
    FloatT &predictedObj,
    FloatT &thetaObj);
  /// Return objective value (without any ClpModel offset) (model may be NULL)
  virtual FloatT objectiveValue(const ClpSimplex *model, const FloatT *solution) const;
  virtual void resize(int newNumberColumns);
  /// Delete columns in  objective
  virtual void deleteSome(int numberToDelete, const int *which);
  /// Scale objective
  virtual void reallyScale(const FloatT *columnScale);
  /** Given a zeroed array sets nonlinear columns to 1.
         Returns number of nonlinear columns
      */
  virtual int markNonlinear(char *which);

  //@}

  ///@name Constructors and destructors
  //@{
  /// Default Constructor
  ClpQuadraticObjective();

  /// Constructor from objective
  ClpQuadraticObjective(const FloatT *linearObjective, int numberColumns,
    const CoinBigIndex *start,
    const int *column, const FloatT *element,
    int numberExtendedColumns_ = -1);

  /** Copy constructor .
         If type is -1 then make sure half symmetric,
         if +1 then make sure full
     */
  ClpQuadraticObjective(const ClpQuadraticObjective &rhs, int type = 0);
  /** Subset constructor.  Duplicates are allowed
         and order is as given.
     */
  ClpQuadraticObjective(const ClpQuadraticObjective &rhs, int numberColumns,
    const int *whichColumns);

  /// Assignment operator
  ClpQuadraticObjective &operator=(const ClpQuadraticObjective &rhs);

  /// Destructor
  virtual ~ClpQuadraticObjective();

  /// Clone
  virtual ClpObjective *clone() const;
  /** Subset clone.  Duplicates are allowed
         and order is as given.
     */
  virtual ClpObjective *subsetClone(int numberColumns,
    const int *whichColumns) const;

  /** Load up quadratic objective.  This is stored as a CoinPackedMatrix */
  void loadQuadraticObjective(const int numberColumns,
    const CoinBigIndex *start,
    const int *column, const FloatT *element,
    int numberExtendedColumns = -1);
  void loadQuadraticObjective(const CoinPackedMatrix &matrix);
  /// Get rid of quadratic objective
  void deleteQuadraticObjective();
  //@}
  ///@name Gets and sets
  //@{
  /// Quadratic objective
  inline CoinPackedMatrix *quadraticObjective() const
  {
    return quadraticObjective_;
  }
  /// Linear objective
  inline FloatT *linearObjective() const
  {
    return objective_;
  }
  /// Length of linear objective which could be bigger
  inline int numberExtendedColumns() const
  {
    return numberExtendedColumns_;
  }
  /// Number of columns in quadratic objective
  inline int numberColumns() const
  {
    return numberColumns_;
  }
  /// If a full or half matrix
  inline bool fullMatrix() const
  {
    return fullMatrix_;
  }
  //@}

  //---------------------------------------------------------------------------

private:
  ///@name Private member data
  /// Quadratic objective
  CoinPackedMatrix *quadraticObjective_;
  /// Objective
  FloatT *objective_;
  /// Gradient
  FloatT *gradient_;
  /// Useful to have number of columns about
  int numberColumns_;
  /// Also length of linear objective which could be bigger
  int numberExtendedColumns_;
  /// True if full symmetric matrix, false if half
  bool fullMatrix_;
  //@}
};

#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

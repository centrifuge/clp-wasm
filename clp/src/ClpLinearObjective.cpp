/* $Id: ClpLinearObjective.cpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include "CoinPragma.hpp"
#include "CoinIndexedVector.hpp"
#include "ClpFactorization.hpp"
#include "ClpSimplex.hpp"
#include "ClpLinearObjective.hpp"
#include "CoinHelperFunctions.hpp"

//#############################################################################
// Constructors / Destructor / Assignment
//#############################################################################

//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
ClpLinearObjective::ClpLinearObjective()
  : ClpObjective()
{
  type_ = 1;
  objective_ = NULL;
  numberColumns_ = 0;
}

//-------------------------------------------------------------------
// Useful Constructor
//-------------------------------------------------------------------
ClpLinearObjective::ClpLinearObjective(const FloatT *objective,
  int numberColumns)
  : ClpObjective()
{
  type_ = 1;
  numberColumns_ = numberColumns;
  objective_ = CoinCopyOfArray(objective, numberColumns_, 0.0);
}

//-------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------
ClpLinearObjective::ClpLinearObjective(const ClpLinearObjective &rhs)
  : ClpObjective(rhs)
{
  numberColumns_ = rhs.numberColumns_;
  objective_ = CoinCopyOfArray(rhs.objective_, numberColumns_);
}
/* Subset constructor.  Duplicates are allowed
   and order is as given.
*/
ClpLinearObjective::ClpLinearObjective(const ClpLinearObjective &rhs,
  int numberColumns,
  const int *whichColumn)
  : ClpObjective(rhs)
{
  objective_ = NULL;
  numberColumns_ = 0;
  if (numberColumns > 0) {
    // check valid lists
    int numberBad = 0;
    int i;
    for (i = 0; i < numberColumns; i++)
      if (whichColumn[i] < 0 || whichColumn[i] >= rhs.numberColumns_)
        numberBad++;
    if (numberBad)
      throw CoinError("bad column list", "subset constructor",
        "ClpLinearObjective");
    numberColumns_ = numberColumns;
    objective_ = new FloatT[numberColumns_];
    for (i = 0; i < numberColumns_; i++)
      objective_[i] = rhs.objective_[whichColumn[i]];
  }
}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
ClpLinearObjective::~ClpLinearObjective()
{
  delete[] objective_;
}

//----------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------
ClpLinearObjective &
ClpLinearObjective::operator=(const ClpLinearObjective &rhs)
{
  if (this != &rhs) {
    ClpObjective::operator=(rhs);
    numberColumns_ = rhs.numberColumns_;
    delete[] objective_;
    objective_ = CoinCopyOfArray(rhs.objective_, numberColumns_);
  }
  return *this;
}

// Returns gradient
FloatT *
ClpLinearObjective::gradient(const ClpSimplex * /*model*/,
  const FloatT * /*solution*/, FloatT &offset,
  bool /*refresh*/,
  int /*includeLinear*/)
{
  // not sure what to do about scaling
  //assert (!model);
  //assert (includeLinear==2); //otherwise need to return all zeros
  offset = 0.0;
  return objective_;
}

/* Returns reduced gradient.Returns an offset (to be added to current one).
 */
FloatT
ClpLinearObjective::reducedGradient(ClpSimplex *model, FloatT *region,
  bool /*useFeasibleCosts*/)
{
  int numberRows = model->numberRows();
  //work space
  CoinIndexedVector *workSpace = model->rowArray(0);

  CoinIndexedVector arrayVector;
  arrayVector.reserve(numberRows + 1);

  int iRow;
#ifdef CLP_DEBUG
  workSpace->checkClear();
#endif
  FloatT *array = arrayVector.denseVector();
  int *index = arrayVector.getIndices();
  int number = 0;
  const FloatT *cost = model->costRegion();
  //assert (!useFeasibleCosts);
  const int *pivotVariable = model->pivotVariable();
  for (iRow = 0; iRow < numberRows; iRow++) {
    int iPivot = pivotVariable[iRow];
    FloatT value = cost[iPivot];
    if (value) {
      array[iRow] = value;
      index[number++] = iRow;
    }
  }
  arrayVector.setNumElements(number);

  int numberColumns = model->numberColumns();

  // Btran basic costs
  FloatT *work = workSpace->denseVector();
  model->factorization()->updateColumnTranspose(workSpace, &arrayVector);
  ClpFillN(work, numberRows, 0.0);
  // now look at dual solution
  FloatT *rowReducedCost = region + numberColumns;
  FloatT *dual = rowReducedCost;
  FloatT *rowCost = model->costRegion(0);
  for (iRow = 0; iRow < numberRows; iRow++) {
    dual[iRow] = array[iRow];
  }
  FloatT *dj = region;
  ClpDisjointCopyN(model->costRegion(1), numberColumns, dj);
  model->transposeTimes(-1.0, dual, dj);
  for (iRow = 0; iRow < numberRows; iRow++) {
    // slack
    FloatT value = dual[iRow];
    value += rowCost[iRow];
    rowReducedCost[iRow] = value;
  }
  return 0.0;
}
/* Returns step length which gives minimum of objective for
   solution + theta * change vector up to maximum theta.

   arrays are numberColumns+numberRows
*/
FloatT
ClpLinearObjective::stepLength(ClpSimplex *model,
  const FloatT *solution,
  const FloatT *change,
  FloatT maximumTheta,
  FloatT &currentObj,
  FloatT &predictedObj,
  FloatT &thetaObj)
{
  const FloatT *cost = model->costRegion();
  FloatT delta = 0.0;
  int numberRows = model->numberRows();
  int numberColumns = model->numberColumns();
  currentObj = 0.0;
  thetaObj = 0.0;
  for (int iColumn = 0; iColumn < numberColumns + numberRows; iColumn++) {
    delta += cost[iColumn] * change[iColumn];
    currentObj += cost[iColumn] * solution[iColumn];
  }
  thetaObj = currentObj + delta * maximumTheta;
  predictedObj = currentObj + delta * maximumTheta;
  if (delta < 0.0) {
    return maximumTheta;
  } else {
    printf("odd linear direction %g\n", (double)delta);
    return 0.0;
  }
}
// Return objective value (without any ClpModel offset) (model may be NULL)
FloatT
ClpLinearObjective::objectiveValue(const ClpSimplex *model, const FloatT *solution) const
{
  const FloatT *cost = objective_;
  if (model && model->costRegion())
    cost = model->costRegion();
  FloatT currentObj = 0.0;
  for (int iColumn = 0; iColumn < numberColumns_; iColumn++) {
    currentObj += cost[iColumn] * solution[iColumn];
  }
  return currentObj;
}
//-------------------------------------------------------------------
// Clone
//-------------------------------------------------------------------
ClpObjective *ClpLinearObjective::clone() const
{
  return new ClpLinearObjective(*this);
}
/* Subset clone.  Duplicates are allowed
   and order is as given.
*/
ClpObjective *
ClpLinearObjective::subsetClone(int numberColumns,
  const int *whichColumns) const
{
  return new ClpLinearObjective(*this, numberColumns, whichColumns);
}
// Resize objective
void ClpLinearObjective::resize(int newNumberColumns)
{
  if (numberColumns_ != newNumberColumns) {
    int i;
    FloatT *newArray = new FloatT[newNumberColumns];
    if (objective_)
      CoinMemcpyN(objective_, CoinMin(newNumberColumns, numberColumns_), newArray);
    delete[] objective_;
    objective_ = newArray;
    for (i = numberColumns_; i < newNumberColumns; i++)
      objective_[i] = 0.0;
    numberColumns_ = newNumberColumns;
  }
}
// Delete columns in  objective
void ClpLinearObjective::deleteSome(int numberToDelete, const int *which)
{
  if (objective_) {
    int i;
    char *deleted = new char[numberColumns_];
    int numberDeleted = 0;
    CoinZeroN(deleted, numberColumns_);
    for (i = 0; i < numberToDelete; i++) {
      int j = which[i];
      if (j >= 0 && j < numberColumns_ && !deleted[j]) {
        numberDeleted++;
        deleted[j] = 1;
      }
    }
    int newNumberColumns = numberColumns_ - numberDeleted;
    FloatT *newArray = new FloatT[newNumberColumns];
    int put = 0;
    for (i = 0; i < numberColumns_; i++) {
      if (!deleted[i]) {
        newArray[put++] = objective_[i];
      }
    }
    delete[] objective_;
    objective_ = newArray;
    delete[] deleted;
    numberColumns_ = newNumberColumns;
  }
}
// Scale objective
void ClpLinearObjective::reallyScale(const FloatT *columnScale)
{
  for (int iColumn = 0; iColumn < numberColumns_; iColumn++) {
    objective_[iColumn] *= columnScale[iColumn];
  }
}

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

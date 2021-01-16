/* $Id: ClpConstraint.cpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2007, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include "CoinPragma.hpp"
#include "ClpSimplex.hpp"
#include "ClpConstraint.hpp"

//#############################################################################
// Constructors / Destructor / Assignment
//#############################################################################

//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
ClpConstraint::ClpConstraint()
  : lastGradient_(NULL)
  , functionValue_(0.0)
  , offset_(0.0)
  , type_(-1)
  , rowNumber_(-1)
{
}

//-------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------
ClpConstraint::ClpConstraint(const ClpConstraint &source)
  : lastGradient_(NULL)
  , functionValue_(source.functionValue_)
  , offset_(source.offset_)
  , type_(source.type_)
  , rowNumber_(source.rowNumber_)
{
}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
ClpConstraint::~ClpConstraint()
{
  delete[] lastGradient_;
}

//----------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------
ClpConstraint &
ClpConstraint::operator=(const ClpConstraint &rhs)
{
  if (this != &rhs) {
    functionValue_ = rhs.functionValue_;
    offset_ = rhs.offset_;
    type_ = rhs.type_;
    rowNumber_ = rhs.rowNumber_;
    delete[] lastGradient_;
    lastGradient_ = NULL;
  }
  return *this;
}
// Constraint function value
FloatT
ClpConstraint::functionValue(const ClpSimplex *model,
  const FloatT *solution,
  bool useScaling,
  bool refresh) const
{
  FloatT offset;
  FloatT value;
  int n = model->numberColumns();
  FloatT *grad = new FloatT[n];
  gradient(model, solution, grad, value, offset, useScaling, refresh);
  delete[] grad;
  return value;
}

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

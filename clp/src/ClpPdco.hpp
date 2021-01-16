/* $Id: ClpPdco.hpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).
/*
   Authors

   John Tomlin

 */
#ifndef ClpPdco_H
#define ClpPdco_H

#include "ClpInterior.hpp"

/** This solves problems in Primal Dual Convex Optimization

    It inherits from ClpInterior.  It has no data of its own and
    is never created - only cast from a ClpInterior object at algorithm time.

*/
class ClpPdco : public ClpInterior {

public:
  /**@name Description of algorithm */
  //@{
  /** Pdco algorithm

         Method


     */

  int pdco();
  // ** Temporary version
  int pdco(ClpPdcoBase *stuff, Options &options, Info &info, Outfo &outfo);

  //@}

  /**@name Functions used in pdco */
  //@{
  /// LSQR
  void lsqr();

  void matVecMult(int, FloatT *, FloatT *);

  void matVecMult(int, CoinDenseVector< FloatT > &, FloatT *);

  void matVecMult(int, CoinDenseVector< FloatT > &, CoinDenseVector< FloatT > &);

  void matVecMult(int, CoinDenseVector< FloatT > *, CoinDenseVector< FloatT > *);

  void getBoundTypes(int *, int *, int *, int **);

  void getGrad(CoinDenseVector< FloatT > &x, CoinDenseVector< FloatT > &grad);

  void getHessian(CoinDenseVector< FloatT > &x, CoinDenseVector< FloatT > &H);

  FloatT getObj(CoinDenseVector< FloatT > &x);

  void matPrecon(FloatT, FloatT *, FloatT *);

  void matPrecon(FloatT, CoinDenseVector< FloatT > &, FloatT *);

  void matPrecon(FloatT, CoinDenseVector< FloatT > &, CoinDenseVector< FloatT > &);

  void matPrecon(FloatT, CoinDenseVector< FloatT > *, CoinDenseVector< FloatT > *);
  //@}
};
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

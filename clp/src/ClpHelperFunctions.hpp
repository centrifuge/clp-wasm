/* $Id: ClpHelperFunctions.hpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#ifndef ClpHelperFunctions_H
#define ClpHelperFunctions_H

#include "ClpConfig.h"
#ifdef HAVE_CMATH
#include <cmath>
#else
#ifdef HAVE_MATH_H
#include <math.h>
#else
#error "don't have header file for math"
#endif
#endif

/**
    Note (JJF) I have added some operations on arrays even though they may
    duplicate CoinDenseVector.  I think the use of templates was a mistake
    as I don't think inline generic code can take as much advantage of
    parallelism or machine architectures or memory hierarchies.

*/

FloatT maximumAbsElement(const FloatT *region, int size);
void setElements(FloatT *region, int size, FloatT value);
void multiplyAdd(const FloatT *region1, int size, FloatT multiplier1,
  FloatT *region2, FloatT multiplier2);
FloatT innerProduct(const FloatT *region1, int size, const FloatT *region2);
void getNorms(const FloatT *region, int size, FloatT &norm1, FloatT &norm2);
#if COIN_LONG_WORK
// For FloatT versions
CoinWorkDouble maximumAbsElement(const CoinWorkDouble *region, int size);
void setElements(CoinWorkDouble *region, int size, CoinWorkDouble value);
void multiplyAdd(const CoinWorkDouble *region1, int size, CoinWorkDouble multiplier1,
  CoinWorkDouble *region2, CoinWorkDouble multiplier2);
CoinWorkDouble innerProduct(const CoinWorkDouble *region1, int size, const CoinWorkDouble *region2);
void getNorms(const CoinWorkDouble *region, int size, CoinWorkDouble &norm1, CoinWorkDouble &norm2);
inline void
CoinMemcpyN(const FloatT *from, const int size, CoinWorkDouble *to)
{
  for (int i = 0; i < size; i++)
    to[i] = from[i];
}
inline void
CoinMemcpyN(const CoinWorkDouble *from, const int size, FloatT *to)
{
  for (int i = 0; i < size; i++)
    to[i] = static_cast< FloatT >(from[i]);
}
inline CoinWorkDouble
CoinMax(const CoinWorkDouble x1, const FloatT x2)
{
  return (x1 > x2) ? x1 : x2;
}
inline CoinWorkDouble
CoinMax(FloatT x1, const CoinWorkDouble x2)
{
  return (x1 > x2) ? x1 : x2;
}
inline CoinWorkDouble
CoinMin(const CoinWorkDouble x1, const FloatT x2)
{
  return (x1 < x2) ? x1 : x2;
}
inline CoinWorkDouble
CoinMin(FloatT x1, const CoinWorkDouble x2)
{
  return (x1 < x2) ? x1 : x2;
}
inline CoinWorkDouble CoinSqrt(CoinWorkDouble x)
{
  return sqrtl(x);
}
#else
inline FloatT CoinSqrt(FloatT x)
{
  return sqrt(x);
}
#endif
/// Trace
#ifdef NDEBUG
#define ClpTraceDebug(expression) \
  {                               \
  }
#else
void ClpTracePrint(std::string fileName, std::string message, int line);
#define ClpTraceDebug(expression)                              \
  {                                                            \
    if (!(expression)) {                                       \
      ClpTracePrint(__FILE__, __STRING(expression), __LINE__); \
    }                                                          \
  }
#endif
/// Following only included if ClpPdco defined
#ifdef ClpPdco_H

inline FloatT pdxxxmerit(int nlow, int nupp, int *low, int *upp, CoinDenseVector< FloatT > &r1,
  CoinDenseVector< FloatT > &r2, CoinDenseVector< FloatT > &rL,
  CoinDenseVector< FloatT > &rU, CoinDenseVector< FloatT > &cL,
  CoinDenseVector< FloatT > &cU)
{

  // Evaluate the merit function for Newton's method.
  // It is the 2-norm of the three sets of residuals.
  FloatT sum1, sum2;
  CoinDenseVector< FloatT > f(6);
  f[0] = r1.twoNorm();
  f[1] = r2.twoNorm();
  sum1 = sum2 = 0.0;
  for (int k = 0; k < nlow; k++) {
    sum1 += rL[low[k]] * rL[low[k]];
    sum2 += cL[low[k]] * cL[low[k]];
  }
  f[2] = sqrt(sum1);
  f[4] = sqrt(sum2);
  sum1 = sum2 = 0.0;
  for (int k = 0; k < nupp; k++) {
    sum1 += rL[upp[k]] * rL[upp[k]];
    sum2 += cL[upp[k]] * cL[upp[k]];
  }
  f[3] = sqrt(sum1);
  f[5] = sqrt(sum2);

  return f.twoNorm();
}

//-----------------------------------------------------------------------
// End private function pdxxxmerit
//-----------------------------------------------------------------------

//function [r1,r2,rL,rU,Pinf,Dinf] =    ...
//      pdxxxresid1( Aname,fix,low,upp, ...
//                   b,bl,bu,d1,d2,grad,rL,rU,x,x1,x2,y,z1,z2 )

inline void pdxxxresid1(ClpPdco *model, const int nlow, const int nupp, const int nfix,
  int *low, int *upp, int *fix,
  CoinDenseVector< FloatT > &b, FloatT *bl, FloatT *bu, FloatT d1, FloatT d2,
  CoinDenseVector< FloatT > &grad, CoinDenseVector< FloatT > &rL,
  CoinDenseVector< FloatT > &rU, CoinDenseVector< FloatT > &x,
  CoinDenseVector< FloatT > &x1, CoinDenseVector< FloatT > &x2,
  CoinDenseVector< FloatT > &y, CoinDenseVector< FloatT > &z1,
  CoinDenseVector< FloatT > &z2, CoinDenseVector< FloatT > &r1,
  CoinDenseVector< FloatT > &r2, FloatT *Pinf, FloatT *Dinf)
{

  // Form residuals for the primal and dual equations.
  // rL, rU are output, but we input them as full vectors
  // initialized (permanently) with any relevant zeros.

  // Get some element pointers for efficiency
  FloatT *x_elts = x.getElements();
  FloatT *r2_elts = r2.getElements();

  for (int k = 0; k < nfix; k++)
    x_elts[fix[k]] = 0;

  r1.clear();
  r2.clear();
  model->matVecMult(1, r1, x);
  model->matVecMult(2, r2, y);
  for (int k = 0; k < nfix; k++)
    r2_elts[fix[k]] = 0;

  r1 = b - r1 - d2 * d2 * y;
  r2 = grad - r2 - z1; // grad includes d1*d1*x
  if (nupp > 0)
    r2 = r2 + z2;

  for (int k = 0; k < nlow; k++)
    rL[low[k]] = bl[low[k]] - x[low[k]] + x1[low[k]];
  for (int k = 0; k < nupp; k++)
    rU[upp[k]] = -bu[upp[k]] + x[upp[k]] + x2[upp[k]];

  FloatT normL = 0.0;
  FloatT normU = 0.0;
  for (int k = 0; k < nlow; k++)
    if (rL[low[k]] > normL)
      normL = rL[low[k]];
  for (int k = 0; k < nupp; k++)
    if (rU[upp[k]] > normU)
      normU = rU[upp[k]];

  *Pinf = CoinMax(normL, normU);
  *Pinf = CoinMax(r1.infNorm(), *Pinf);
  *Dinf = r2.infNorm();
  *Pinf = CoinMax(*Pinf, 1e-99);
  *Dinf = CoinMax(*Dinf, 1e-99);
}

//-----------------------------------------------------------------------
// End private function pdxxxresid1
//-----------------------------------------------------------------------

//function [cL,cU,center,Cinf,Cinf0] = ...
//      pdxxxresid2( mu,low,upp,cL,cU,x1,x2,z1,z2 )

inline void pdxxxresid2(FloatT mu, int nlow, int nupp, int *low, int *upp,
  CoinDenseVector< FloatT > &cL, CoinDenseVector< FloatT > &cU,
  CoinDenseVector< FloatT > &x1, CoinDenseVector< FloatT > &x2,
  CoinDenseVector< FloatT > &z1, CoinDenseVector< FloatT > &z2,
  FloatT *center, FloatT *Cinf, FloatT *Cinf0)
{

  // Form residuals for the complementarity equations.
  // cL, cU are output, but we input them as full vectors
  // initialized (permanently) with any relevant zeros.
  // Cinf  is the complementarity residual for X1 z1 = mu e, etc.
  // Cinf0 is the same for mu=0 (i.e., for the original problem).

  FloatT maxXz = TOO_SMALL_FLOAT;
  FloatT minXz = TOO_BIG_FLOAT;

  FloatT *x1_elts = x1.getElements();
  FloatT *z1_elts = z1.getElements();
  FloatT *cL_elts = cL.getElements();
  for (int k = 0; k < nlow; k++) {
    FloatT x1z1 = x1_elts[low[k]] * z1_elts[low[k]];
    cL_elts[low[k]] = mu - x1z1;
    if (x1z1 > maxXz)
      maxXz = x1z1;
    if (x1z1 < minXz)
      minXz = x1z1;
  }

  FloatT *x2_elts = x2.getElements();
  FloatT *z2_elts = z2.getElements();
  FloatT *cU_elts = cU.getElements();
  for (int k = 0; k < nupp; k++) {
    FloatT x2z2 = x2_elts[upp[k]] * z2_elts[upp[k]];
    cU_elts[upp[k]] = mu - x2z2;
    if (x2z2 > maxXz)
      maxXz = x2z2;
    if (x2z2 < minXz)
      minXz = x2z2;
  }

  maxXz = CoinMax(maxXz, 1e-99);
  minXz = CoinMax(minXz, 1e-99);
  *center = maxXz / minXz;

  FloatT normL = 0.0;
  FloatT normU = 0.0;
  for (int k = 0; k < nlow; k++)
    if (cL_elts[low[k]] > normL)
      normL = cL_elts[low[k]];
  for (int k = 0; k < nupp; k++)
    if (cU_elts[upp[k]] > normU)
      normU = cU_elts[upp[k]];
  *Cinf = CoinMax(normL, normU);
  *Cinf0 = maxXz;
}
//-----------------------------------------------------------------------
// End private function pdxxxresid2
//-----------------------------------------------------------------------

inline FloatT pdxxxstep(CoinDenseVector< FloatT > &x, CoinDenseVector< FloatT > &dx)
{

  // Assumes x > 0.
  // Finds the maximum step such that x + step*dx >= 0.

  FloatT step = 1e+20;

  int n = x.size();
  FloatT *x_elts = x.getElements();
  FloatT *dx_elts = dx.getElements();
  for (int k = 0; k < n; k++)
    if (dx_elts[k] < 0)
      if ((x_elts[k] / (-dx_elts[k])) < step)
        step = x_elts[k] / (-dx_elts[k]);
  return step;
}
//-----------------------------------------------------------------------
// End private function pdxxxstep
//-----------------------------------------------------------------------

inline FloatT pdxxxstep(int nset, int *set, CoinDenseVector< FloatT > &x, CoinDenseVector< FloatT > &dx)
{

  // Assumes x > 0.
  // Finds the maximum step such that x + step*dx >= 0.

  FloatT step = 1e+20;

  int n = x.size();
  FloatT *x_elts = x.getElements();
  FloatT *dx_elts = dx.getElements();
  for (int k = 0; k < n; k++)
    if (dx_elts[k] < 0)
      if ((x_elts[k] / (-dx_elts[k])) < step)
        step = x_elts[k] / (-dx_elts[k]);
  return step;
}
//-----------------------------------------------------------------------
// End private function pdxxxstep
//-----------------------------------------------------------------------
#endif
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

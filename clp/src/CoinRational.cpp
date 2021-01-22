// Authors: Matthew Saltzman and Ted Ralphs
// Copyright 2015, Matthew Saltzman and Ted Ralphs
// Licensed under the Eclipse Public License 1.0

#include <algorithm>
#include <cmath>
#ifdef __clang__
//labs() is in cstdlib with clang
#include <cstdlib>
#include <cstdio>
#endif
#include <cassert>

#include "CoinRational.hpp"

#include "CoinHelperFunctions.hpp"

// Based on Python code from
// http://www.johndcook.com/blog/2010/10/20/best-rational-approximation/
// (with permission).
//
// Returns closest (or almost, anyway) rational to val with denominator less
// than or equal to maxdnom.  Return value is true if within tolerance, false
// otherwise.
bool CoinRational::nearestRational_(FloatT val, FloatT maxdelta, long maxdnom)
{
  FloatT intpart;
  FloatT fracpart = CoinAbs(modf(val, &intpart));
  // Consider using remainder() instead?

  long a = 0, b = 1, c = 1, d = 1;
#define DEBUG_X 1
#if DEBUG_X
  bool shouldBeOK = false;
#endif
  while (b <= maxdnom && d <= maxdnom) {
    FloatT mediant = (a + c) / (FloatT(b + d));

    if (CoinAbs(fracpart - mediant) < maxdelta) {
#if DEBUG_X
      shouldBeOK = true;
#endif
      if (b + d <= maxdnom * 2) { // seems more accurate (always true)
        numerator_ = a + c;
        denominator_ = b + d;
        break;
      } else if (d > b) {
        numerator_ = c;
        denominator_ = d;
        break;
      } else {
        numerator_ = a;
        denominator_ = b;
        break;
      }
    } else if (fracpart > mediant) {
      a = a + c;
      b = b + d;
    } else {
      c = a + c;
      d = b + d;
    }
    if (b > maxdnom) {
      numerator_ = c;
      denominator_ = d;
    } else {
      numerator_ = a;
      denominator_ = b;
    }
  }

#if DEBUG_X
  if (shouldBeOK) {
    FloatT inaccuracy = CoinAbs(fracpart - numerator_ / FloatT(denominator_));
    assert(inaccuracy <= maxdelta);
  }
#endif
  numerator_ += (long)(CoinAbs(intpart) * fd(denominator_));
  if (val < 0)
    numerator_ *= -1;
#if DEBUG_X > 1
  if (shouldBeOK) {
    printf("val %g is %ld/%ld to accuracy %g\n", val, numerator_, denominator_,
      CoinAbs(val - numerator_ / FloatT(denominator_)));
  }
#endif
  return CoinAbs(val - numerator_ / FloatT(denominator_)) <= maxdelta;
}

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

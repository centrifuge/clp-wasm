/* $Id: CoinFinite.cpp 2083 2019-01-06 19:38:09Z unxusr $ */
// Copyright (C) 2011, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

#include "CoinFinite.hpp"
#include "CoinUtilsConfig.h"

#ifdef HAVE_CMATH
#include <cmath>
#else
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#endif

#ifdef HAVE_CIEEEFP
#include <cieeefp>
#else
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#endif

bool CoinFinite(FloatT val)
{
  return mp::isfinite(val);
}

bool CoinIsnan(FloatT val)
{
  return mp::isnan(val);
}

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

/* $Id: CoinFinite.hpp 2083 2019-01-06 19:38:09Z unxusr $ */
// Copyright (C) 2002, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

/* Defines COIN_DBL_MAX and relatives and provides CoinFinite and CoinIsnan. */

#ifndef CoinFinite_H
#define CoinFinite_H

#include "floatdef.h"

#include <limits>

//=============================================================================
// Smallest positive FloatT value and Plus infinity (FloatT and int)

#if 1
const FloatT COIN_DBL_MIN = (std::numeric_limits< FloatT >::min)();
const FloatT COIN_DBL_MAX = (std::numeric_limits< FloatT >::max)();
const int COIN_INT_MAX = (std::numeric_limits< int >::max)();
const FloatT COIN_INT_MAX_AS_DOUBLE = (std::numeric_limits< int >::max)();
#else
#define COIN_DBL_MIN (std::numeric_limits< FloatT >::min())
#define COIN_DBL_MAX (std::numeric_limits< FloatT >::max())
#define COIN_INT_MAX (std::numeric_limits< int >::max())
#define COIN_INT_MAX_AS_DOUBLE (std::numeric_limits< int >::max())
#endif

/** checks if a FloatT value is finite (not infinity and not NaN) */
extern bool CoinFinite(FloatT val);

/** checks if a FloatT value is not a number */
extern bool CoinIsnan(FloatT val);

#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

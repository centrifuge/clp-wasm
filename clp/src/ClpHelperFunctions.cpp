/* $Id: ClpHelperFunctions.cpp 2385 2019-01-06 19:43:06Z unxusr $ */
// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.
// This code is licensed under the terms of the Eclipse Public License (EPL).

/*
    Note (JJF) I have added some operations on arrays even though they may
    duplicate CoinDenseVector.  I think the use of templates was a mistake
    as I don't think inline generic code can take as much advantage of
    parallelism or machine architectures or memory hierarchies.

*/
#include <cfloat>
#include <cstdlib>
#include <cmath>
#include "CoinHelperFunctions.hpp"
#include "CoinTypes.hpp"

FloatT
maximumAbsElement(const FloatT *region, int size)
{
  int i;
  FloatT maxValue = 0.0;
  for (i = 0; i < size; i++)
    maxValue = CoinMax(maxValue, CoinAbs(region[i]));
  return maxValue;
}
void setElements(FloatT *region, int size, FloatT value)
{
  int i;
  for (i = 0; i < size; i++)
    region[i] = value;
}
void multiplyAdd(const FloatT *region1, int size, FloatT multiplier1,
  FloatT *region2, FloatT multiplier2)
{
  int i;
  if (multiplier1 == 1.0) {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] + multiplier2 * region2[i];
    }
  } else if (multiplier1 == -1.0) {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] + multiplier2 * region2[i];
    }
  } else if (multiplier1 == 0.0) {
    if (multiplier2 == 1.0) {
      // nothing to do
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = 0.0;
    } else {
      for (i = 0; i < size; i++)
        region2[i] = multiplier2 * region2[i];
    }
  } else {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] + multiplier2 * region2[i];
    }
  }
}
FloatT
innerProduct(const FloatT *region1, int size, const FloatT *region2)
{
  int i;
  FloatT value = 0.0;
  for (i = 0; i < size; i++)
    value += region1[i] * region2[i];
  return value;
}
void getNorms(const FloatT *region, int size, FloatT &norm1, FloatT &norm2)
{
  norm1 = 0.0;
  norm2 = 0.0;
  int i;
  for (i = 0; i < size; i++) {
    norm2 += region[i] * region[i];
    norm1 = CoinMax(norm1, CoinAbs(region[i]));
  }
}
#ifndef NDEBUG
#include "ClpModel.hpp"
#include "ClpMessage.hpp"
ClpModel *clpTraceModel = NULL; // Set to trap messages
void ClpTracePrint(std::string fileName, std::string message, int lineNumber)
{
  if (!clpTraceModel) {
    std::cout << fileName << ":" << lineNumber << " : \'"
              << message << "\' failed." << std::endl;
  } else {
    char line[1000];
    sprintf(line, "%s: %d : \'%s\' failed.", fileName.c_str(), lineNumber, message.c_str());
    clpTraceModel->messageHandler()->message(CLP_GENERAL_WARNING, clpTraceModel->messages())
      << line
      << CoinMessageEol;
  }
}
#endif
#if COIN_LONG_WORK
// For FloatT versions
CoinWorkDouble
maximumAbsElement(const CoinWorkDouble *region, int size)
{
  int i;
  CoinWorkDouble maxValue = 0.0;
  for (i = 0; i < size; i++)
    maxValue = CoinMax(maxValue, CoinAbs(region[i]));
  return maxValue;
}
void setElements(CoinWorkDouble *region, int size, CoinWorkDouble value)
{
  int i;
  for (i = 0; i < size; i++)
    region[i] = value;
}
void multiplyAdd(const CoinWorkDouble *region1, int size, CoinWorkDouble multiplier1,
  CoinWorkDouble *region2, CoinWorkDouble multiplier2)
{
  int i;
  if (multiplier1 == 1.0) {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = region1[i] + multiplier2 * region2[i];
    }
  } else if (multiplier1 == -1.0) {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = -region1[i] + multiplier2 * region2[i];
    }
  } else if (multiplier1 == 0.0) {
    if (multiplier2 == 1.0) {
      // nothing to do
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = -region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = 0.0;
    } else {
      for (i = 0; i < size; i++)
        region2[i] = multiplier2 * region2[i];
    }
  } else {
    if (multiplier2 == 1.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] + region2[i];
    } else if (multiplier2 == -1.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] - region2[i];
    } else if (multiplier2 == 0.0) {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i];
    } else {
      for (i = 0; i < size; i++)
        region2[i] = multiplier1 * region1[i] + multiplier2 * region2[i];
    }
  }
}
CoinWorkDouble
innerProduct(const CoinWorkDouble *region1, int size, const CoinWorkDouble *region2)
{
  int i;
  CoinWorkDouble value = 0.0;
  for (i = 0; i < size; i++)
    value += region1[i] * region2[i];
  return value;
}
void getNorms(const CoinWorkDouble *region, int size, CoinWorkDouble &norm1, CoinWorkDouble &norm2)
{
  norm1 = 0.0;
  norm2 = 0.0;
  int i;
  for (i = 0; i < size; i++) {
    norm2 += region[i] * region[i];
    norm1 = CoinMax(norm1, CoinAbs(region[i]));
  }
}
#endif
#ifdef DEBUG_MEMORY
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*NEW_HANDLER)();
static NEW_HANDLER new_handler; // function to call if `new' fails (cf. ARM p. 281)

// Allocate storage.
void *
operator new(size_t size)
{
  void *p;
  for (;;) {
    p = malloc(size);
    if (p)
      break; // success
    else if (new_handler)
      new_handler(); // failure - try again (allow user to release some storage first)
    else
      break; // failure - no retry
  }
  if (size > 1000000)
    printf("Allocating memory of size %d\n", size);
  return p;
}

// Deallocate storage.
void operator delete(void *p)
{
  free(p);
  return;
}
void operator delete[](void *p)
{
  free(p);
  return;
}
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

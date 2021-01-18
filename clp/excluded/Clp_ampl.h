/* $Id: Clp_ampl.h 1573 2011-01-05 01:12:36Z lou $ */
/*
  Copyright (C) 2006, International Business Machines Corporation and others.
  All Rights Reserved.

  This code is licensed under the terms of the Eclipse Public License (EPL).
*/

#ifndef AmplInterface_H
#define AmplInterface_H
typedef struct {
  int numberRows;
  int numberColumns;
  int numberBinary;
  int numberIntegers; /* non binary */
  int numberSos;
  int numberElements;
  int numberArguments;
  int problemStatus;
  FloatT direction;
  FloatT offset;
  FloatT objValue;
  FloatT *objective;
  FloatT *rowLower;
  FloatT *rowUpper;
  FloatT *columnLower;
  FloatT *columnUpper;
#if COIN_BIG_INDEX == 0
  int *starts;
#else
  long unsigned int *starts;
#endif
  int *rows;
  FloatT *elements;
  FloatT *primalSolution;
  FloatT *dualSolution;
  int *columnStatus;
  int *rowStatus;
  int *priorities;
  int *branchDirection;
  FloatT *pseudoDown;
  FloatT *pseudoUp;
  char *sosType;
  int *sosPriority;
  int *sosStart;
  int *sosIndices;
  FloatT *sosReference;
  int *cut;
  int *special;
  char **arguments;
  char buffer[300];
  int logLevel;
  int nonLinear;
} ampl_info;
#ifdef __cplusplus
extern "C" {
#endif
/* return nonzero if bad */
int readAmpl(ampl_info *info, int argc, char **argv,
  void **coinModel);
/* frees some input arrays */
void freeArrays1(ampl_info *info);
/* frees rest */
void freeArrays2(ampl_info *info);
/* frees fake arguments */
void freeArgs(ampl_info *info);
/* writes ampl stuff */
void writeAmpl(ampl_info *info);
/* objective precision */
int ampl_obj_prec();
#ifdef __cplusplus
}
#endif
#endif

/* vi: softtabstop=2 shiftwidth=2 expandtab tabstop=2
*/

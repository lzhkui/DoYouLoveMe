//
// File: y_emxutil.h
//
// MATLAB Coder version            : 2.7
// C/C++ source code generated on  : 05-Jul-2016 16:59:34
//
#ifndef __Y_EMXUTIL_H__
#define __Y_EMXUTIL_H__

// Include Files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "y_types.h"

// Function Declarations
extern void b_emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);
extern void emxEnsureCapacity(emxArray__common *emxArray, int oldNumel, int
  elementSize);
extern void emxFree_real_T(emxArray_real_T **pEmxArray);
extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

#endif

//
// File trailer for y_emxutil.h
//
// [EOF]
//

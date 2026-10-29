/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: RedLineGuidance_emxutil.h
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

#ifndef REDLINEGUIDANCE_EMXUTIL_H
#define REDLINEGUIDANCE_EMXUTIL_H

/* Include Files */
#include "RedLineGuidance_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void emxEnsureCapacity_int32_T(emxArray_int32_T *emxArray, int oldNumel);

extern void emxEnsureCapacity_real_T(emxArray_real_T *emxArray, int oldNumel);

extern void emxFree_int32_T(emxArray_int32_T **pEmxArray);

extern void emxFree_real_T(emxArray_real_T **pEmxArray);

extern void emxInit_int32_T(emxArray_int32_T **pEmxArray);

extern void emxInit_real_T(emxArray_real_T **pEmxArray);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for RedLineGuidance_emxutil.h
 *
 * [EOF]
 */

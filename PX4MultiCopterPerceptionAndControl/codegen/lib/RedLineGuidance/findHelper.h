/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: findHelper.h
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

#ifndef FINDHELPER_H
#define FINDHELPER_H

/* Include Files */
#include "RedLineGuidance_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
int b_findHelper(const boolean_T x[5760], int i_data[], int j_data[],
                 int *j_size);

int c_findHelper(const boolean_T x[7360], int i_data[], int j_data[],
                 int *j_size);

void findHelper(const boolean_T x[19200], emxArray_int32_T *i,
                emxArray_int32_T *j);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for findHelper.h
 *
 * [EOF]
 */

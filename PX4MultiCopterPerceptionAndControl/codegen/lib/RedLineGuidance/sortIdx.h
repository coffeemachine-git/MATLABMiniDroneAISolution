/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sortIdx.h
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

#ifndef SORTIDX_H
#define SORTIDX_H

/* Include Files */
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void b_merge_block(int idx_data[], double x_data[], int offset, int n,
                   int preSortLevel, int iwork_data[], double xwork_data[]);

void merge_block(int idx_data[], double x_data[], int offset, int n,
                 int preSortLevel, int iwork_data[], double xwork_data[]);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for sortIdx.h
 *
 * [EOF]
 */

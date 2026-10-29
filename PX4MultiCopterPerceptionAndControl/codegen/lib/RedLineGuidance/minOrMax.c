/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: minOrMax.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "minOrMax.h"
#include "RedLineGuidance_types.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const emxArray_real_T *x
 * Return Type  : double
 */
double maximum(const emxArray_real_T *x)
{
  const double *x_data;
  double ex;
  int k;
  int last;
  x_data = x->data;
  last = x->size[0];
  if ((unsigned short)(x->size[0] - 1) + 1 <= 2) {
    if ((unsigned short)(x->size[0] - 1) == 0) {
      ex = x_data[0];
    } else {
      ex = x_data[x->size[0] - 1];
      if (!(x_data[0] < ex)) {
        ex = x_data[0];
      }
    }
  } else {
    ex = x_data[0];
    for (k = 2; k <= last; k++) {
      double d;
      d = x_data[k - 1];
      if (ex < d) {
        ex = d;
      }
    }
  }
  return ex;
}

/*
 * Arguments    : const emxArray_real_T *x
 * Return Type  : double
 */
double minimum(const emxArray_real_T *x)
{
  const double *x_data;
  double ex;
  int k;
  int last;
  x_data = x->data;
  last = x->size[0];
  if ((unsigned short)(x->size[0] - 1) + 1 <= 2) {
    if ((unsigned short)(x->size[0] - 1) == 0) {
      ex = x_data[0];
    } else {
      ex = x_data[x->size[0] - 1];
      if (!(x_data[0] > ex)) {
        ex = x_data[0];
      }
    }
  } else {
    ex = x_data[0];
    for (k = 2; k <= last; k++) {
      double d;
      d = x_data[k - 1];
      if (ex > d) {
        ex = d;
      }
    }
  }
  return ex;
}

/*
 * File trailer for minOrMax.c
 *
 * [EOF]
 */

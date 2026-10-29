/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: findHelper.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "findHelper.h"
#include "RedLineGuidance_emxutil.h"
#include "RedLineGuidance_types.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const boolean_T x[5760]
 *                int i_data[]
 *                int j_data[]
 *                int *j_size
 * Return Type  : int
 */
int b_findHelper(const boolean_T x[5760], int i_data[], int j_data[],
                 int *j_size)
{
  int i_size;
  int ii;
  int jj;
  i_size = 0;
  ii = 1;
  jj = 1;
  while (jj <= 160) {
    boolean_T guard1;
    guard1 = false;
    if (x[(ii + 36 * (jj - 1)) - 1]) {
      i_size++;
      i_data[i_size - 1] = ii;
      j_data[i_size - 1] = jj;
      if (i_size >= 5760) {
        break;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
    if (guard1) {
      ii++;
      if (ii > 36) {
        ii = 1;
        jj++;
      }
    }
  }
  if (i_size < 1) {
    i_size = 0;
  }
  *j_size = i_size;
  return i_size;
}

/*
 * Arguments    : const boolean_T x[7360]
 *                int i_data[]
 *                int j_data[]
 *                int *j_size
 * Return Type  : int
 */
int c_findHelper(const boolean_T x[7360], int i_data[], int j_data[],
                 int *j_size)
{
  int i_size;
  int ii;
  int jj;
  i_size = 0;
  ii = 1;
  jj = 1;
  while (jj <= 160) {
    boolean_T guard1;
    guard1 = false;
    if (x[(ii + 46 * (jj - 1)) - 1]) {
      i_size++;
      i_data[i_size - 1] = ii;
      j_data[i_size - 1] = jj;
      if (i_size >= 7360) {
        break;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
    if (guard1) {
      ii++;
      if (ii > 46) {
        ii = 1;
        jj++;
      }
    }
  }
  if (i_size < 1) {
    i_size = 0;
  }
  *j_size = i_size;
  return i_size;
}

/*
 * Arguments    : const boolean_T x[19200]
 *                emxArray_int32_T *i
 *                emxArray_int32_T *j
 * Return Type  : void
 */
void findHelper(const boolean_T x[19200], emxArray_int32_T *i,
                emxArray_int32_T *j)
{
  int idx;
  int ii;
  int jj;
  int *i_data;
  int *j_data;
  idx = 0;
  ii = i->size[0];
  i->size[0] = 19200;
  emxEnsureCapacity_int32_T(i, ii);
  i_data = i->data;
  ii = j->size[0];
  j->size[0] = 19200;
  emxEnsureCapacity_int32_T(j, ii);
  j_data = j->data;
  ii = 1;
  jj = 1;
  while (jj <= 160) {
    boolean_T guard1;
    guard1 = false;
    if (x[(ii + 120 * (jj - 1)) - 1]) {
      idx++;
      i_data[idx - 1] = ii;
      j_data[idx - 1] = jj;
      if (idx >= 19200) {
        break;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
    if (guard1) {
      ii++;
      if (ii > 120) {
        ii = 1;
        jj++;
      }
    }
  }
  if (idx < 1) {
    idx = 0;
  }
  ii = i->size[0];
  i->size[0] = idx;
  emxEnsureCapacity_int32_T(i, ii);
  ii = j->size[0];
  j->size[0] = idx;
  emxEnsureCapacity_int32_T(j, ii);
}

/*
 * File trailer for findHelper.c
 *
 * [EOF]
 */

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: sort.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "sort.h"
#include "rt_nonfinite.h"
#include "sortIdx.h"

/* Function Definitions */
/*
 * Arguments    : double x_data[]
 *                const int *x_size
 *                int idx_data[]
 * Return Type  : int
 */
int b_sort(double x_data[], const int *x_size, int idx_data[])
{
  static double vwork_data[7360];
  static double xwork_data[7360];
  int b;
  int b_b;
  int b_k;
  int dim;
  int idx_size;
  int j;
  int k;
  int vstride;
  int vwork_size;
  dim = 2;
  if (*x_size != 1) {
    dim = 1;
  }
  if (dim <= 1) {
    vwork_size = *x_size;
  } else {
    vwork_size = 1;
  }
  idx_size = *x_size;
  vstride = 1;
  dim -= 2;
  for (k = 0; k <= dim; k++) {
    vstride *= *x_size;
  }
  for (j = 0; j < vstride; j++) {
    double x4[4];
    int iidx_data[7360];
    int iwork_data[7360];
    int i2;
    int i4;
    int ib;
    int nBlocks;
    int offset;
    short idx4[4];
    x4[0] = 0.0;
    idx4[0] = 0;
    x4[1] = 0.0;
    idx4[1] = 0;
    x4[2] = 0.0;
    idx4[2] = 0;
    x4[3] = 0.0;
    idx4[3] = 0;
    ib = 0;
    for (k = 0; k < vwork_size; k++) {
      double d;
      d = x_data[j + k * vstride];
      vwork_data[k] = d;
      iidx_data[k] = 0;
      iwork_data[k] = 0;
      ib++;
      idx4[ib - 1] = (short)(k + 1);
      x4[ib - 1] = d;
      if (ib == 4) {
        double d1;
        if (x4[0] <= x4[1]) {
          dim = 1;
          i2 = 2;
        } else {
          dim = 2;
          i2 = 1;
        }
        if (x4[2] <= x4[3]) {
          ib = 3;
          i4 = 4;
        } else {
          ib = 4;
          i4 = 3;
        }
        d = x4[dim - 1];
        d1 = x4[ib - 1];
        if (d <= d1) {
          if (x4[i2 - 1] <= d1) {
            nBlocks = dim;
            offset = i2;
            dim = ib;
            i2 = i4;
          } else if (x4[i2 - 1] <= x4[i4 - 1]) {
            nBlocks = dim;
            offset = ib;
            dim = i2;
            i2 = i4;
          } else {
            nBlocks = dim;
            offset = ib;
            dim = i4;
          }
        } else if (d <= x4[i4 - 1]) {
          if (x4[i2 - 1] <= x4[i4 - 1]) {
            nBlocks = ib;
            offset = dim;
            dim = i2;
            i2 = i4;
          } else {
            nBlocks = ib;
            offset = dim;
            dim = i4;
          }
        } else {
          nBlocks = ib;
          offset = i4;
        }
        iidx_data[k - 3] = idx4[nBlocks - 1];
        iidx_data[k - 2] = idx4[offset - 1];
        iidx_data[k - 1] = idx4[dim - 1];
        iidx_data[k] = idx4[i2 - 1];
        vwork_data[k - 3] = x4[nBlocks - 1];
        vwork_data[k - 2] = x4[offset - 1];
        vwork_data[k - 1] = x4[dim - 1];
        vwork_data[k] = x4[i2 - 1];
        ib = 0;
      }
    }
    if (ib > 0) {
      signed char perm[4];
      perm[1] = 0;
      perm[2] = 0;
      perm[3] = 0;
      if (ib == 1) {
        perm[0] = 1;
      } else if (ib == 2) {
        if (x4[0] <= x4[1]) {
          perm[0] = 1;
          perm[1] = 2;
        } else {
          perm[0] = 2;
          perm[1] = 1;
        }
      } else if (x4[0] <= x4[1]) {
        if (x4[1] <= x4[2]) {
          perm[0] = 1;
          perm[1] = 2;
          perm[2] = 3;
        } else if (x4[0] <= x4[2]) {
          perm[0] = 1;
          perm[1] = 3;
          perm[2] = 2;
        } else {
          perm[0] = 3;
          perm[1] = 1;
          perm[2] = 2;
        }
      } else if (x4[0] <= x4[2]) {
        perm[0] = 2;
        perm[1] = 1;
        perm[2] = 3;
      } else if (x4[1] <= x4[2]) {
        perm[0] = 2;
        perm[1] = 3;
        perm[2] = 1;
      } else {
        perm[0] = 3;
        perm[1] = 2;
        perm[2] = 1;
      }
      for (k = 0; k < ib; k++) {
        dim = (vwork_size - ib) + k;
        i2 = perm[k];
        iidx_data[dim] = idx4[i2 - 1];
        vwork_data[dim] = x4[i2 - 1];
      }
    }
    dim = 2;
    if (vwork_size > 1) {
      if (vwork_size >= 256) {
        nBlocks = vwork_size >> 8;
        for (b = 0; b < nBlocks; b++) {
          double xwork[256];
          short iwork[256];
          offset = (b << 8) - 1;
          for (b_b = 0; b_b < 6; b_b++) {
            int bLen;
            int bLen2;
            int nPairs;
            bLen = 1 << (b_b + 2);
            bLen2 = bLen << 1;
            nPairs = 256 >> (b_b + 3);
            for (b_k = 0; b_k < nPairs; b_k++) {
              i2 = (offset + b_k * bLen2) + 1;
              for (k = 0; k < bLen2; k++) {
                dim = i2 + k;
                iwork[k] = (short)iidx_data[dim];
                xwork[k] = vwork_data[dim];
              }
              i4 = 0;
              ib = bLen;
              dim = i2 - 1;
              int exitg1;
              do {
                exitg1 = 0;
                dim++;
                if (xwork[i4] <= xwork[ib]) {
                  iidx_data[dim] = iwork[i4];
                  vwork_data[dim] = xwork[i4];
                  if (i4 + 1 < bLen) {
                    i4++;
                  } else {
                    exitg1 = 1;
                  }
                } else {
                  iidx_data[dim] = iwork[ib];
                  vwork_data[dim] = xwork[ib];
                  if (ib + 1 < bLen2) {
                    ib++;
                  } else {
                    dim -= i4;
                    for (k = i4 + 1; k <= bLen; k++) {
                      ib = dim + k;
                      iidx_data[ib] = iwork[k - 1];
                      vwork_data[ib] = xwork[k - 1];
                    }
                    exitg1 = 1;
                  }
                }
              } while (exitg1 == 0);
            }
          }
        }
        dim = nBlocks << 8;
        i2 = vwork_size - dim;
        if (i2 > 0) {
          b_merge_block(iidx_data, vwork_data, dim, i2, 2, iwork_data,
                        xwork_data);
        }
        dim = 8;
      }
      b_merge_block(iidx_data, vwork_data, 0, vwork_size, dim, iwork_data,
                    xwork_data);
    }
    for (k = 0; k < vwork_size; k++) {
      dim = j + k * vstride;
      x_data[dim] = vwork_data[k];
      idx_data[dim] = iidx_data[k];
    }
  }
  return idx_size;
}

/*
 * Arguments    : double x_data[]
 *                const int *x_size
 *                int idx_data[]
 * Return Type  : int
 */
int sort(double x_data[], const int *x_size, int idx_data[])
{
  static double vwork_data[7360];
  static double xwork_data[7360];
  int b;
  int b_b;
  int b_k;
  int dim;
  int idx_size;
  int j;
  int k;
  int vstride;
  int vwork_size;
  dim = 2;
  if (*x_size != 1) {
    dim = 1;
  }
  if (dim <= 1) {
    vwork_size = *x_size;
  } else {
    vwork_size = 1;
  }
  idx_size = *x_size;
  vstride = 1;
  dim -= 2;
  for (k = 0; k <= dim; k++) {
    vstride *= *x_size;
  }
  for (j = 0; j < vstride; j++) {
    double x4[4];
    int iidx_data[7360];
    int iwork_data[7360];
    int i2;
    int i4;
    int ib;
    int nBlocks;
    int offset;
    short idx4[4];
    x4[0] = 0.0;
    idx4[0] = 0;
    x4[1] = 0.0;
    idx4[1] = 0;
    x4[2] = 0.0;
    idx4[2] = 0;
    x4[3] = 0.0;
    idx4[3] = 0;
    ib = 0;
    for (k = 0; k < vwork_size; k++) {
      double d;
      d = x_data[j + k * vstride];
      vwork_data[k] = d;
      iidx_data[k] = 0;
      iwork_data[k] = 0;
      ib++;
      idx4[ib - 1] = (short)(k + 1);
      x4[ib - 1] = d;
      if (ib == 4) {
        double d1;
        if (x4[0] >= x4[1]) {
          dim = 1;
          i2 = 2;
        } else {
          dim = 2;
          i2 = 1;
        }
        if (x4[2] >= x4[3]) {
          ib = 3;
          i4 = 4;
        } else {
          ib = 4;
          i4 = 3;
        }
        d = x4[dim - 1];
        d1 = x4[ib - 1];
        if (d >= d1) {
          if (x4[i2 - 1] >= d1) {
            nBlocks = dim;
            offset = i2;
            dim = ib;
            i2 = i4;
          } else if (x4[i2 - 1] >= x4[i4 - 1]) {
            nBlocks = dim;
            offset = ib;
            dim = i2;
            i2 = i4;
          } else {
            nBlocks = dim;
            offset = ib;
            dim = i4;
          }
        } else if (d >= x4[i4 - 1]) {
          if (x4[i2 - 1] >= x4[i4 - 1]) {
            nBlocks = ib;
            offset = dim;
            dim = i2;
            i2 = i4;
          } else {
            nBlocks = ib;
            offset = dim;
            dim = i4;
          }
        } else {
          nBlocks = ib;
          offset = i4;
        }
        iidx_data[k - 3] = idx4[nBlocks - 1];
        iidx_data[k - 2] = idx4[offset - 1];
        iidx_data[k - 1] = idx4[dim - 1];
        iidx_data[k] = idx4[i2 - 1];
        vwork_data[k - 3] = x4[nBlocks - 1];
        vwork_data[k - 2] = x4[offset - 1];
        vwork_data[k - 1] = x4[dim - 1];
        vwork_data[k] = x4[i2 - 1];
        ib = 0;
      }
    }
    if (ib > 0) {
      signed char perm[4];
      perm[1] = 0;
      perm[2] = 0;
      perm[3] = 0;
      if (ib == 1) {
        perm[0] = 1;
      } else if (ib == 2) {
        if (x4[0] >= x4[1]) {
          perm[0] = 1;
          perm[1] = 2;
        } else {
          perm[0] = 2;
          perm[1] = 1;
        }
      } else if (x4[0] >= x4[1]) {
        if (x4[1] >= x4[2]) {
          perm[0] = 1;
          perm[1] = 2;
          perm[2] = 3;
        } else if (x4[0] >= x4[2]) {
          perm[0] = 1;
          perm[1] = 3;
          perm[2] = 2;
        } else {
          perm[0] = 3;
          perm[1] = 1;
          perm[2] = 2;
        }
      } else if (x4[0] >= x4[2]) {
        perm[0] = 2;
        perm[1] = 1;
        perm[2] = 3;
      } else if (x4[1] >= x4[2]) {
        perm[0] = 2;
        perm[1] = 3;
        perm[2] = 1;
      } else {
        perm[0] = 3;
        perm[1] = 2;
        perm[2] = 1;
      }
      for (k = 0; k < ib; k++) {
        dim = (vwork_size - ib) + k;
        i2 = perm[k];
        iidx_data[dim] = idx4[i2 - 1];
        vwork_data[dim] = x4[i2 - 1];
      }
    }
    dim = 2;
    if (vwork_size > 1) {
      if (vwork_size >= 256) {
        nBlocks = vwork_size >> 8;
        for (b = 0; b < nBlocks; b++) {
          double xwork[256];
          short iwork[256];
          offset = (b << 8) - 1;
          for (b_b = 0; b_b < 6; b_b++) {
            int bLen;
            int bLen2;
            int nPairs;
            bLen = 1 << (b_b + 2);
            bLen2 = bLen << 1;
            nPairs = 256 >> (b_b + 3);
            for (b_k = 0; b_k < nPairs; b_k++) {
              i2 = (offset + b_k * bLen2) + 1;
              for (k = 0; k < bLen2; k++) {
                dim = i2 + k;
                iwork[k] = (short)iidx_data[dim];
                xwork[k] = vwork_data[dim];
              }
              i4 = 0;
              ib = bLen;
              dim = i2 - 1;
              int exitg1;
              do {
                exitg1 = 0;
                dim++;
                if (xwork[i4] >= xwork[ib]) {
                  iidx_data[dim] = iwork[i4];
                  vwork_data[dim] = xwork[i4];
                  if (i4 + 1 < bLen) {
                    i4++;
                  } else {
                    exitg1 = 1;
                  }
                } else {
                  iidx_data[dim] = iwork[ib];
                  vwork_data[dim] = xwork[ib];
                  if (ib + 1 < bLen2) {
                    ib++;
                  } else {
                    dim -= i4;
                    for (k = i4 + 1; k <= bLen; k++) {
                      ib = dim + k;
                      iidx_data[ib] = iwork[k - 1];
                      vwork_data[ib] = xwork[k - 1];
                    }
                    exitg1 = 1;
                  }
                }
              } while (exitg1 == 0);
            }
          }
        }
        dim = nBlocks << 8;
        i2 = vwork_size - dim;
        if (i2 > 0) {
          merge_block(iidx_data, vwork_data, dim, i2, 2, iwork_data,
                      xwork_data);
        }
        dim = 8;
      }
      merge_block(iidx_data, vwork_data, 0, vwork_size, dim, iwork_data,
                  xwork_data);
    }
    for (k = 0; k < vwork_size; k++) {
      dim = j + k * vstride;
      x_data[dim] = vwork_data[k];
      idx_data[dim] = iidx_data[k];
    }
  }
  return idx_size;
}

/*
 * File trailer for sort.c
 *
 * [EOF]
 */

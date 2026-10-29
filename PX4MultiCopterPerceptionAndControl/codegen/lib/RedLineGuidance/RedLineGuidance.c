/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: RedLineGuidance.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "RedLineGuidance.h"
#include "RedLineGuidance_data.h"
#include "RedLineGuidance_emxutil.h"
#include "RedLineGuidance_initialize.h"
#include "RedLineGuidance_types.h"
#include "atan2.h"
#include "combineVectorElements.h"
#include "findHelper.h"
#include "mean.h"
#include "minOrMax.h"
#include "rt_nonfinite.h"
#include "sort.h"
#include "omp.h"
#include <math.h>
#include <string.h>

/* Variable Definitions */
static double state;

static double z_cmd;

static boolean_T is_init_not_empty;

static double loss_timer;

static double takeoff_timer;

static double align_timer;

static double last_turn_bias;

/* Function Definitions */
/*
 * REDLINEGUIDANCE Robust closed-loop perception and guidance for MATLAB
 * Minidrone Red Line Follower
 *
 *  Fully autonomous guidance system designed to follow arbitrary red line
 * tracks regardless of geometry: sharp turns (90 deg, acute, obtuse, hairpin),
 * smooth curves, line gaps, and distractors, with precision alignment and
 * landing on the red destination circle.
 *
 *  Inputs:
 *    image:      120x160x3 uint8 downward camera frame
 *    currentPos: [X; Y; Z; yaw] drone state in NED frame
 *
 *  Output:
 *    setpoint:   [X_cmd; Y_cmd; Z_cmd; yaw_cmd] in NED frame
 *
 * Arguments    : const unsigned char image[57600]
 *                const double currentPos[4]
 *                double setpoint[4]
 * Return Type  : void
 */
void RedLineGuidance(const unsigned char image[57600],
                     const double currentPos[4], double setpoint[4])
{
  static double y_data[7360];
  static double cB_data[5760];
  static double c_ii_data[5760];
  static double b_y0;
  static double circle_x;
  static double circle_y;
  static double last_valid_yaw;
  static double x0;
  static double x_cmd;
  static double y_cmd;
  static double yaw0;
  static double yaw_cmd;
  static int c_jj_data[7360];
  static int d_ii_data[7360];
  static int rM_data[7360];
  emxArray_int32_T *ii;
  emxArray_int32_T *jj;
  emxArray_real_T b_cB_data;
  emxArray_real_T *cAll;
  emxArray_real_T *rAll;
  double b_rM_data[35];
  double c_all;
  double c_base;
  double c_span;
  double max_c;
  double min_c;
  double min_r;
  double r_all;
  double r_base;
  double r_span;
  double scale;
  double *rAll_data;
  int b_ii_data[5760];
  int b_jj_data[5760];
  int b_k;
  int b_nz;
  int cB_size;
  int c_nz;
  int i1;
  int i2;
  int i3;
  int ii_size;
  int jj_size;
  int k;
  int nz;
  int rM_size;
  int *ii_data;
  int *jj_data;
  short i;
  unsigned char B[19200];
  unsigned char G[19200];
  unsigned char R[19200];
  unsigned char u;
  unsigned char u1;
  unsigned char u2;
  boolean_T is_red[19200];
  boolean_T c_is_red[7360];
  boolean_T b_is_red[5760];
  boolean_T isSideBranch;
  if (!isInitialized_RedLineGuidance) {
    RedLineGuidance_initialize();
  }
  /*  States: */
  /*  0: TAKEOFF / CLIMB */
  /*  1: TRACK (Lookahead pursuit for straight lines, curves, and sharp corners)
   */
  /*  2: GAP_RECOVER (Crawl along last heading if line temporarily lost) */
  /*  3: ALIGN_CIRCLE (Precision 2D centering over destination circle) */
  /*  4: LAND (Active descent with closed-loop centering) */
  /*  5: LANDED (Touchdown hold) */
  /*  Camera frame period (5 Hz) */
  /*  Cruise altitude (0.8m above ground) */
  if (!is_init_not_empty) {
    x0 = currentPos[0];
    b_y0 = currentPos[1];
    yaw0 = currentPos[3];
    x_cmd = x0;
    y_cmd = b_y0;
    yaw_cmd = yaw0;
    last_valid_yaw = yaw0;
    circle_x = x0;
    circle_y = b_y0;
    is_init_not_empty = true;
  }
  /*  Extract RGB color channels */
  memcpy(&R[0], &image[0], 19200U * sizeof(unsigned char));
  memcpy(&G[0], &image[19200], 19200U * sizeof(unsigned char));
  memcpy(&B[0], &image[38400], 19200U * sizeof(unsigned char));
  /*  1. Robust Red Pixel Segmentation */
  /*  Rejects gravel floor (grey/green), yellow distractors (R~G), blue
   * distractors, and shadows */
#pragma omp parallel for num_threads(omp_get_max_threads()) private(i, u, u1,  \
                                                                        u2)

  for (k = 0; k < 19200; k++) {
    u = G[k];
    u1 = B[k];
    i = (short)(u + u1);
    i = (short)fmax(i, 1.0);
    u2 = R[k];
    is_red[k] = ((u2 > 75) && (u2 - u > 25) && (u2 - u1 > 25) &&
                 ((double)u2 / (double)i > 0.65));
  }
  nz = is_red[0];
  for (b_k = 0; b_k < 19199; b_k++) {
    nz += is_red[b_k + 1];
  }
  /*  Altitude-adaptive metric scaling (meters per pixel) */
  scale = 0.00432 * (fmax(-currentPos[2], 0.15) / 0.8);
  /*  2. Image Feature Extraction */
  /*  Camera frame: */
  /*  Row 1 = Forward (+X body), Row 120 = Rear (-X body), Row 60 = Nadir */
  /*  Col 1 = Left (-Y body), Col 160 = Right (+Y body), Col 80 = Centerline */
  emxInit_int32_T(&ii);
  emxInit_int32_T(&jj);
  findHelper(is_red, ii, jj);
  jj_data = jj->data;
  ii_data = ii->data;
  emxInit_real_T(&rAll);
  b_nz = ii->size[0];
  c_nz = rAll->size[0];
  rAll->size[0] = ii->size[0];
  emxEnsureCapacity_real_T(rAll, c_nz);
  rAll_data = rAll->data;
  for (i1 = 0; i1 < b_nz; i1++) {
    rAll_data[i1] = ii_data[i1];
  }
  emxFree_int32_T(&ii);
  emxInit_real_T(&cAll);
  b_nz = jj->size[0];
  c_nz = cAll->size[0];
  cAll->size[0] = jj->size[0];
  emxEnsureCapacity_real_T(cAll, c_nz);
  rAll_data = cAll->data;
  for (b_k = 0; b_k < b_nz; b_k++) {
    rAll_data[b_k] = jj_data[b_k];
  }
  emxFree_int32_T(&jj);
  if (rAll->size[0] != 0) {
    c_all = mean(cAll);
    r_all = mean(rAll);
    min_c = minimum(cAll);
    max_c = maximum(cAll);
    min_r = minimum(rAll);
    r_span = maximum(rAll) - min_r;
    c_span = max_c - min_c;
  } else {
    c_all = 80.0;
    r_all = 60.0;
    min_c = 80.0;
    max_c = 80.0;
    min_r = 60.0;
    r_span = 0.0;
    c_span = 0.0;
  }
  emxFree_real_T(&cAll);
  emxFree_real_T(&rAll);
  /*  Base point directly under drone (rows 50 to 85) */
  for (b_k = 0; b_k < 160; b_k++) {
    for (i1 = 0; i1 < 36; i1++) {
      b_is_red[i1 + 36 * b_k] = is_red[(i1 + 120 * b_k) + 49];
    }
  }
  b_nz = b_findHelper(b_is_red, b_ii_data, b_jj_data, &c_nz);
  cB_size = c_nz;
  for (b_k = 0; b_k < c_nz; b_k++) {
    cB_data[b_k] = b_jj_data[b_k];
  }
  if (cB_size != 0) {
    b_cB_data.data = &cB_data[0];
    b_cB_data.size = &cB_size;
    b_cB_data.allocatedSize = 5760;
    b_cB_data.numDimensions = 1;
    b_cB_data.canFreeData = false;
    c_base = mean(&b_cB_data);
    ii_size = b_nz;
    for (b_k = 0; b_k < b_nz; b_k++) {
      c_ii_data[b_k] = b_ii_data[b_k];
    }
    b_cB_data.data = &c_ii_data[0];
    b_cB_data.size = &ii_size;
    b_cB_data.allocatedSize = 5760;
    b_cB_data.numDimensions = 1;
    b_cB_data.canFreeData = false;
    r_base = mean(&b_cB_data) + 49.0;
  } else {
    c_base = c_all;
    r_base = r_all;
  }
  /*  Forward path (rows 10 to 45) */
  for (b_k = 0; b_k < 160; b_k++) {
    for (i1 = 0; i1 < 36; i1++) {
      b_is_red[i1 + 36 * b_k] = is_red[(i1 + 120 * b_k) + 9];
    }
  }
  b_nz = b_findHelper(b_is_red, b_ii_data, b_jj_data, &c_nz);
  cB_size = c_nz;
  for (b_k = 0; b_k < c_nz; b_k++) {
    cB_data[b_k] = b_jj_data[b_k];
  }
  /*  Lateral / Mid path (rows 30 to 75) */
  for (i1 = 0; i1 < 160; i1++) {
    for (b_k = 0; b_k < 46; b_k++) {
      c_is_red[b_k + 46 * i1] = is_red[(b_k + 120 * i1) + 29];
    }
  }
  c_nz = c_findHelper(c_is_red, d_ii_data, c_jj_data, &jj_size);
  if (c_nz - 1 >= 0) {
    memcpy(&rM_data[0], &d_ii_data[0], (unsigned int)c_nz * sizeof(int));
  }
  /*  Side branch detection: red path extends to side image boundaries */
  if ((max_c >= 155.0) || (min_c <= 6.0) || (c_span > 65.0)) {
    isSideBranch = true;
  } else {
    isSideBranch = false;
  }
  /*  Landing circle detection: */
  /*  Compact isolated circular disc (diameter ~46 px, area > 600 px), */
  /*  balanced aspect ratio, DOES NOT touch side boundaries, and no forward line
   * continuing */
  if (!isSideBranch && (nz > 600) && (c_span > 22.0) && (r_span > 20.0) &&
      (fabs(c_span - r_span) < 25.0) && (cB_size < 100) && (min_r > 10.0) &&
      (min_c > 12.0) && (max_c < 148.0)) {
    isSideBranch = true;
  } else {
    isSideBranch = false;
  }
  /*  3. Finite State Machine */
  if (state == 0.0) {
    /*  TAKEOFF / CLIMB */
    z_cmd = -0.8;
    x_cmd = x0;
    y_cmd = b_y0;
    yaw_cmd = yaw0;
    last_valid_yaw = yaw0;
    takeoff_timer += 0.2;
    if ((currentPos[2] <= -0.65) && (takeoff_timer >= 2.0)) {
      state = 1.0;
      /*  Enter line tracking */
      loss_timer = 0.0;
    }
  } else if (state == 1.0) {
    /*  LINE TRACKING (Straight, Curves, and Corners) */
    z_cmd = -0.8;
    if (isSideBranch) {
      state = 3.0;
      /*  Enter ALIGN_CIRCLE */
      align_timer = 0.0;
      circle_x = currentPos[0];
      circle_y = currentPos[1];
    } else if (nz < 35) {
      loss_timer += 0.2;
      if (loss_timer > 0.4) {
        state = 2.0;
        /*  Enter GAP_RECOVER */
      }
    } else {
      loss_timer = 0.0;
      /*  Lookahead target selection */
      if (cB_size > 45) {
        /*  Case A: Continuous path ahead (straight or gentle curve) */
        ii_size = b_nz;
        for (i1 = 0; i1 < b_nz; i1++) {
          c_ii_data[i1] = b_ii_data[i1];
        }
        b_cB_data.data = &c_ii_data[0];
        b_cB_data.size = &ii_size;
        b_cB_data.allocatedSize = 5760;
        b_cB_data.numDimensions = 1;
        b_cB_data.canFreeData = false;
        min_c = mean(&b_cB_data) + 9.0;
        b_cB_data.data = &cB_data[0];
        b_cB_data.size = &cB_size;
        b_cB_data.allocatedSize = 5760;
        b_cB_data.numDimensions = 1;
        b_cB_data.canFreeData = false;
        r_span = mean(&b_cB_data);
        last_turn_bias = 0.0;

        /*  Case B: Forward path ends! Corner or sharp turn ahead. */
        /*  Search lateral region for the turn branch (rows 30:75) */
      } else if (jj_size != 0) {
        if (jj_size < 1200) {
          for (i2 = 0; i2 < jj_size; i2++) {
            c_is_red[i2] = (c_jj_data[i2] > 85);
          }
        } else {
#pragma omp parallel for num_threads(omp_get_max_threads())

          for (i2 = 0; i2 < jj_size; i2++) {
            c_is_red[i2] = (c_jj_data[i2] > 85);
          }
        }
        b_nz = combineVectorElements(c_is_red, jj_size);
        if (jj_size < 1200) {
          for (i3 = 0; i3 < jj_size; i3++) {
            c_is_red[i3] = (c_jj_data[i3] < 75);
          }
        } else {
#pragma omp parallel for num_threads(omp_get_max_threads())

          for (i3 = 0; i3 < jj_size; i3++) {
            c_is_red[i3] = (c_jj_data[i3] < 75);
          }
        }
        c_nz = combineVectorElements(c_is_red, jj_size);
        /*  Use directional hysteresis while negotiating sharp turn */
        if ((b_nz > (double)c_nz + 15.0) ||
            ((last_turn_bias == 1.0) && (b_nz >= c_nz))) {
          /*  Turn Right */
          last_turn_bias = 1.0;
          for (b_k = 0; b_k < jj_size; b_k++) {
            y_data[b_k] = c_jj_data[b_k];
          }
          c_nz = sort(y_data, &jj_size, d_ii_data);
          for (b_k = 0; b_k < c_nz; b_k++) {
            y_data[b_k] = d_ii_data[b_k];
          }
          c_nz = (int)fmin(35.0, c_nz);
          rM_size = c_nz;
          for (i1 = 0; i1 < c_nz; i1++) {
            b_rM_data[i1] = rM_data[(int)y_data[i1] - 1];
          }
          b_cB_data.data = &b_rM_data[0];
          b_cB_data.size = &rM_size;
          b_cB_data.allocatedSize = 35;
          b_cB_data.numDimensions = 1;
          b_cB_data.canFreeData = false;
          min_c = mean(&b_cB_data) + 29.0;
          rM_size = c_nz;
          for (i1 = 0; i1 < c_nz; i1++) {
            b_rM_data[i1] = c_jj_data[(int)y_data[i1] - 1];
          }
          b_cB_data.data = &b_rM_data[0];
          b_cB_data.size = &rM_size;
          b_cB_data.allocatedSize = 35;
          b_cB_data.numDimensions = 1;
          b_cB_data.canFreeData = false;
          r_span = mean(&b_cB_data);
        } else {
          /*  Turn Left */
          last_turn_bias = -1.0;
          for (b_k = 0; b_k < jj_size; b_k++) {
            y_data[b_k] = c_jj_data[b_k];
          }
          c_nz = b_sort(y_data, &jj_size, d_ii_data);
          for (b_k = 0; b_k < c_nz; b_k++) {
            y_data[b_k] = d_ii_data[b_k];
          }
          c_nz = (int)fmin(35.0, c_nz);
          rM_size = c_nz;
          for (i1 = 0; i1 < c_nz; i1++) {
            b_rM_data[i1] = rM_data[(int)y_data[i1] - 1];
          }
          b_cB_data.data = &b_rM_data[0];
          b_cB_data.size = &rM_size;
          b_cB_data.allocatedSize = 35;
          b_cB_data.numDimensions = 1;
          b_cB_data.canFreeData = false;
          min_c = mean(&b_cB_data) + 29.0;
          rM_size = c_nz;
          for (i1 = 0; i1 < c_nz; i1++) {
            b_rM_data[i1] = c_jj_data[(int)y_data[i1] - 1];
          }
          b_cB_data.data = &b_rM_data[0];
          b_cB_data.size = &rM_size;
          b_cB_data.allocatedSize = 35;
          b_cB_data.numDimensions = 1;
          b_cB_data.canFreeData = false;
          r_span = mean(&b_cB_data);
        }
      } else {
        min_c = r_base;
        r_span = c_base;
      }
      /*  Vector from base (under drone) to lookahead target: */
      /*  positive forward (pixels) */
      /*  positive right (pixels) */
      /*  Heading error relative to track: */
      max_c = b_atan2(r_span - c_base, fmax(r_base - min_c, 1.0));
      /*  rad */
      /*  Lateral position error of line under drone: */
      min_c = (c_base - 80.0) * scale;
      /*  meters (+right, -left) */
      /*  Adaptive forward cruise speed: */
      /*  Automatically brakes smoothly when entering sharp turns (psi_err -> 90
       * deg -> v_fwd -> 0) */
      /*  Accelerates back to cruise speed when aligned with track */
      /*  m/s */
      r_all = fmax(0.0, cos(max_c));
      /*  Smooth heading rate command: */
      /*  rad/s */
      yaw_cmd += 1.5 * max_c * 0.2;
      last_valid_yaw = yaw_cmd;
      /*  Body frame displacement setpoints: */
      min_r =
          0.25 * (r_all * r_all) * fmax(0.15, 1.0 - fabs(min_c) / 0.15) * 0.2;
      min_c = 0.5 * min_c * 0.2 + 0.1 * (r_span - 80.0) * scale * 0.2;
      /*  Convert body displacement to NED world frame: */
      max_c = cos(yaw_cmd);
      r_all = sin(yaw_cmd);
      x_cmd += min_r * max_c - min_c * r_all;
      y_cmd += min_r * r_all + min_c * max_c;
    }
  } else if (state == 2.0) {
    /*  GAP RECOVERY (Crawl forward across track gap) */
    x_cmd += 0.016 * cos(last_valid_yaw);
    y_cmd += 0.016 * sin(last_valid_yaw);
    z_cmd = -0.8;
    yaw_cmd = last_valid_yaw;
    if (nz > 70) {
      state = 1.0;
      /*  Reacquired red track */
      loss_timer = 0.0;
    }
  } else if (state == 3.0) {
    /*  ALIGN OVER CIRCLE (Precision 2D centering) */
    min_r = (60.0 - r_all) * scale;
    min_c = (c_all - 80.0) * scale;
    max_c = cos(last_valid_yaw);
    r_all = sin(last_valid_yaw);
    /*  Smooth proportional nudge toward circle center */
    circle_x = currentPos[0] + 0.5 * (min_r * max_c - min_c * r_all);
    circle_y = currentPos[1] + 0.5 * (min_r * r_all + min_c * max_c);
    x_cmd = circle_x;
    y_cmd = circle_y;
    z_cmd = -0.8;
    yaw_cmd = last_valid_yaw;
    align_timer += 0.2;
    if ((align_timer >= 0.8) && (fabs(min_r) < 0.04) && (fabs(min_c) < 0.04)) {
      state = 4.0;
      /*  Centered! Start descent */
    }
  } else if (state == 4.0) {
    /*  CONTROLLED LANDING DESCENT (Active centering while descending) */
    min_r = (60.0 - r_all) * scale;
    min_c = (c_all - 80.0) * scale;
    max_c = cos(last_valid_yaw);
    r_all = sin(last_valid_yaw);
    if (nz > 150) {
      circle_x = currentPos[0] + 0.3 * (min_r * max_c - min_c * r_all);
      circle_y = currentPos[1] + 0.3 * (min_r * r_all + min_c * max_c);
    }
    x_cmd = circle_x;
    y_cmd = circle_y;
    yaw_cmd = last_valid_yaw;
    z_cmd = fmin(0.04, z_cmd + 0.04000000000000001);
    /*  Smooth 0.20 m/s descent */
    if (currentPos[2] > -0.06) {
      state = 5.0;
      /*  Touchdown */
    }
  } else if (state == 5.0) {
    /*  LANDED */
    x_cmd = circle_x;
    y_cmd = circle_y;
    z_cmd = 0.05;
    /*  Hold firmly on ground */
    yaw_cmd = last_valid_yaw;
  }
  setpoint[0] = x_cmd;
  setpoint[1] = y_cmd;
  setpoint[2] = z_cmd;
  setpoint[3] = yaw_cmd;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void RedLineGuidance_init(void)
{
  is_init_not_empty = false;
  state = 0.0;
  z_cmd = -0.8;
  last_turn_bias = 0.0;
  loss_timer = 0.0;
  takeoff_timer = 0.0;
  align_timer = 0.0;
}

/*
 * File trailer for RedLineGuidance.c
 *
 * [EOF]
 */

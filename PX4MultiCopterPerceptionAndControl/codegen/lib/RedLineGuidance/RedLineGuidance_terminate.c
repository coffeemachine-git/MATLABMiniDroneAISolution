/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: RedLineGuidance_terminate.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "RedLineGuidance_terminate.h"
#include "RedLineGuidance_data.h"
#include "rt_nonfinite.h"
#include "omp.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void RedLineGuidance_terminate(void)
{
  omp_destroy_nest_lock(&RedLineGuidance_nestLockGlobal);
  isInitialized_RedLineGuidance = false;
}

/*
 * File trailer for RedLineGuidance_terminate.c
 *
 * [EOF]
 */

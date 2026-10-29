/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: RedLineGuidance_initialize.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/* Include Files */
#include "RedLineGuidance_initialize.h"
#include "RedLineGuidance.h"
#include "RedLineGuidance_data.h"
#include "rt_nonfinite.h"
#include "omp.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void RedLineGuidance_initialize(void)
{
  omp_init_nest_lock(&RedLineGuidance_nestLockGlobal);
  RedLineGuidance_init();
  isInitialized_RedLineGuidance = true;
}

/*
 * File trailer for RedLineGuidance_initialize.c
 *
 * [EOF]
 */

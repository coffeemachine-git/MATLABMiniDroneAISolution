/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_RedLineGuidance_api.h
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

#ifndef _CODER_REDLINEGUIDANCE_API_H
#define _CODER_REDLINEGUIDANCE_API_H

/* Include Files */
#include "emlrt.h"
#include "mex.h"
#include "tmwtypes.h"
#include <string.h>

/* Variable Declarations */

#ifdef __cplusplus
extern "C" {
#endif

extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void RedLineGuidance(const uint8_T image[57600], const real_T currentPos[4],
                     real_T setpoint[4]);

void RedLineGuidance_api(const mxArray *const prhs[2], const mxArray **plhs);

void RedLineGuidance_atexit(void);

void RedLineGuidance_initialize(void);

void RedLineGuidance_terminate(void);

void RedLineGuidance_xil_shutdown(void);

void RedLineGuidance_xil_terminate(void);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for _coder_RedLineGuidance_api.h
 *
 * [EOF]
 */

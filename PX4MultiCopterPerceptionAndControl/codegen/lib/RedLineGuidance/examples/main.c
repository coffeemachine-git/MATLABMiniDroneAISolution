/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: main.c
 *
 * MATLAB Coder version            : 26.2
 * C/C++ source code generated on  : 27-Oct-2026 06:49:13
 */

/*************************************************************************/
/* This automatically generated example C main file shows how to call    */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/

/* Include Files */
#include "main.h"
#include "RedLineGuidance.h"
#include "RedLineGuidance_initialize.h"
#include "RedLineGuidance_terminate.h"
#include "rt_nonfinite.h"

/* Function Declarations */
static void argInit_120x160x3_uint8_T(unsigned char result[57600]);

static void argInit_4x1_real_T(double result[4]);

static double argInit_real_T(void);

static unsigned char argInit_uint8_T(void);

/* Function Definitions */
/*
 * Arguments    : unsigned char result[57600]
 * Return Type  : void
 */
static void argInit_120x160x3_uint8_T(unsigned char result[57600])
{
  int idx0;
  int idx1;
  int idx2;
  /* Loop over the array to initialize each element. */
  for (idx0 = 0; idx0 < 120; idx0++) {
    for (idx1 = 0; idx1 < 160; idx1++) {
      for (idx2 = 0; idx2 < 3; idx2++) {
        /* Set the value of the array element.
Change this value to the value that the application requires. */
        result[(idx0 + 120 * idx1) + 19200 * idx2] = argInit_uint8_T();
      }
    }
  }
}

/*
 * Arguments    : double result[4]
 * Return Type  : void
 */
static void argInit_4x1_real_T(double result[4])
{
  int idx0;
  /* Loop over the array to initialize each element. */
  for (idx0 = 0; idx0 < 4; idx0++) {
    /* Set the value of the array element.
Change this value to the value that the application requires. */
    result[idx0] = argInit_real_T();
  }
}

/*
 * Arguments    : void
 * Return Type  : double
 */
static double argInit_real_T(void)
{
  return 0.0;
}

/*
 * Arguments    : void
 * Return Type  : unsigned char
 */
static unsigned char argInit_uint8_T(void)
{
  return 0U;
}

/*
 * Arguments    : int argc
 *                char **argv
 * Return Type  : int
 */
int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  /* Initialize the application.
You do not need to do this more than one time. */
  RedLineGuidance_initialize();
  /* Invoke the entry-point functions.
You can call entry-point functions multiple times. */
  main_RedLineGuidance();
  /* Terminate the application.
You do not need to do this more than one time. */
  RedLineGuidance_terminate();
  return 0;
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void main_RedLineGuidance(void)
{
  static unsigned char uv[57600];
  double dv[4];
  double setpoint[4];
  /* Initialize function 'RedLineGuidance' input arguments. */
  /* Initialize function input argument 'image'. */
  /* Initialize function input argument 'currentPos'. */
  /* Call the entry-point 'RedLineGuidance'. */
  argInit_120x160x3_uint8_T(uv);
  argInit_4x1_real_T(dv);
  RedLineGuidance(uv, dv, setpoint);
}

/*
 * File trailer for main.c
 *
 * [EOF]
 */

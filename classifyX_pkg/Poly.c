/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: Poly.c
 *
 * MATLAB Coder version            : 4.3
 * C/C++ source code generated on  : 23-Jan-2020 12:13:50
 */

/* Include Files */
#include "Poly.h"
#include "classifyX.h"
#include <string.h>

/* Function Definitions */

/*
 * Arguments    : const double svT_data[]
 *                const int svT_size[2]
 *                const double x[12]
 *                double kernelProduct_data[]
 *                int kernelProduct_size[2]
 * Return Type  : void
 */
void Poly(const double svT_data[], const int svT_size[2], const double x[12],
          double kernelProduct_data[], int kernelProduct_size[2])
{
  int n;
  int j;
  double temp;
  int k;
  double temp_data[13];
  n = svT_size[1];
  kernelProduct_size[1] = (signed char)svT_size[1];
  for (j = 0; j < n; j++) {
    temp = 0.0;
    for (k = 0; k < 12; k++) {
      temp += x[k] * svT_data[j + svT_size[1] * k];
    }

    kernelProduct_data[j] = temp;
  }

  n = kernelProduct_size[1] - 1;
  for (j = 0; j <= n; j++) {
    kernelProduct_data[j]++;
  }

  n = kernelProduct_size[1];
  if (0 <= n - 1) {
    memcpy(&temp_data[0], &kernelProduct_data[0], n * sizeof(double));
  }

  kernelProduct_size[0] = 1;
  n = kernelProduct_size[1] - 1;
  for (j = 0; j <= n; j++) {
    kernelProduct_data[j] *= temp_data[j];
  }
}

/*
 * File trailer for Poly.c
 *
 * [EOF]
 */

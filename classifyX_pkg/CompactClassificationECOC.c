/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: CompactClassificationECOC.c
 *
 * MATLAB Coder version            : 4.3
 * C/C++ source code generated on  : 23-Jan-2020 12:13:50
 */

/* Include Files */
#include "CompactClassificationECOC.h"
#include "Poly.h"
#include "classifyX.h"
#include <math.h>
#include <string.h>

/* Function Definitions */

/*
 * Arguments    : const double obj_ClassNames[4]
 *                const c_classreg_learning_coder_class obj_BinaryLearners[6]
 *                const double obj_CodingMatrix[24]
 *                const double Xin[12]
 * Return Type  : double
 */
double c_CompactClassificationECOC_pre(const double obj_ClassNames[4], const
  c_classreg_learning_coder_class obj_BinaryLearners[6], const double
  obj_CodingMatrix[24], const double Xin[12])
{
  double M[24];
  boolean_T b;
  int i;
  int idx;
  int k;
  double b_obj_BinaryLearners;
  double negloss[4];
  boolean_T y;
  double Xnorm[12];
  int bcoef;
  boolean_T x[12];
  int acoef;
  boolean_T exitg1;
  double pscore[6];
  int obj_BinaryLearners_size[2];
  double obj_BinaryLearners_data[156];
  signed char tmp_data[12];
  double b_tmp_data[13];
  int tmp_size[2];
  signed char unnamed_idx_1;
  signed char c_tmp_data[12];
  int c_size_idx_1;
  double c_data[12];
  signed char d_tmp_data[12];
  memcpy(&M[0], &obj_CodingMatrix[0], 24U * sizeof(double));
  b = true;
  i = 0;
  for (idx = 0; idx < 24; idx++) {
    if (b) {
      b = false;
      i = idx % 4 * 6 + idx / 4;
    } else {
      i += 6;
      if (i > 23) {
        i -= 23;
      }
    }

    if (M[i] == 0.0) {
      M[i] = 0.0;
    }
  }

  for (idx = 0; idx < 6; idx++) {
    for (i = 0; i < 12; i++) {
      Xnorm[i] = Xin[i];
      x[i] = (obj_BinaryLearners[idx].Mu[i] == 0.0);
    }

    y = true;
    k = 0;
    exitg1 = false;
    while ((!exitg1) && (k < 12)) {
      if (!x[k]) {
        y = false;
        exitg1 = true;
      } else {
        k++;
      }
    }

    if (!y) {
      for (k = 0; k < 12; k++) {
        Xnorm[k] = Xin[k] - obj_BinaryLearners[idx].Mu[k];
      }
    }

    for (i = 0; i < 12; i++) {
      x[i] = (obj_BinaryLearners[idx].Sigma[i] == 1.0);
    }

    y = true;
    k = 0;
    exitg1 = false;
    while ((!exitg1) && (k < 12)) {
      if (!x[k]) {
        y = false;
        exitg1 = true;
      } else {
        k++;
      }
    }

    if (!y) {
      k = 0;
      for (bcoef = 0; bcoef < 12; bcoef++) {
        if (obj_BinaryLearners[idx].Sigma[bcoef] > 0.0) {
          k++;
        }
      }

      acoef = 0;
      for (bcoef = 0; bcoef < 12; bcoef++) {
        if (obj_BinaryLearners[idx].Sigma[bcoef] > 0.0) {
          tmp_data[acoef] = (signed char)(bcoef + 1);
          acoef++;
        }
      }

      for (i = 0; i < k; i++) {
        c_tmp_data[i] = (signed char)(tmp_data[i] - 1);
      }

      unnamed_idx_1 = (signed char)k;
      c_size_idx_1 = (signed char)k;
      if (unnamed_idx_1 != 0) {
        acoef = (k != 1);
        bcoef = (k != 1);
        i = unnamed_idx_1 - 1;
        for (k = 0; k <= i; k++) {
          c_data[k] = Xnorm[c_tmp_data[acoef * k]] / obj_BinaryLearners[idx].
            Sigma[tmp_data[bcoef * k] - 1];
        }
      }

      k = 0;
      for (bcoef = 0; bcoef < 12; bcoef++) {
        if (obj_BinaryLearners[idx].Sigma[bcoef] > 0.0) {
          k++;
        }
      }

      acoef = 0;
      for (bcoef = 0; bcoef < 12; bcoef++) {
        if (obj_BinaryLearners[idx].Sigma[bcoef] > 0.0) {
          d_tmp_data[acoef] = (signed char)(bcoef + 1);
          acoef++;
        }
      }

      for (i = 0; i < k; i++) {
        c_tmp_data[i] = (signed char)(d_tmp_data[i] - 1);
      }

      for (i = 0; i < c_size_idx_1; i++) {
        Xnorm[c_tmp_data[i]] = c_data[i];
      }
    }

    obj_BinaryLearners_size[1] = obj_BinaryLearners[idx].SupportVectorsT.size[1];
    obj_BinaryLearners_size[0] = 12;
    bcoef = obj_BinaryLearners[idx].SupportVectorsT.size[1] *
      obj_BinaryLearners[idx].SupportVectorsT.size[0];
    for (i = 0; i < bcoef; i++) {
      obj_BinaryLearners_data[i] = obj_BinaryLearners[idx]
        .SupportVectorsT.data[i] / obj_BinaryLearners[idx].Scale;
    }

    for (i = 0; i < 12; i++) {
      Xnorm[i] /= obj_BinaryLearners[idx].Scale;
    }

    Poly(obj_BinaryLearners_data, obj_BinaryLearners_size, Xnorm, b_tmp_data,
         tmp_size);
    bcoef = obj_BinaryLearners[idx].Alpha.size[0];
    b_obj_BinaryLearners = 0.0;
    for (i = 0; i < bcoef; i++) {
      b_obj_BinaryLearners += obj_BinaryLearners[idx].Alpha.data[i] *
        b_tmp_data[i];
    }

    pscore[idx] = b_obj_BinaryLearners + obj_BinaryLearners[idx].Bias;
  }

  for (k = 0; k < 4; k++) {
    b_obj_BinaryLearners = 0.0;
    bcoef = 0;
    for (acoef = 0; acoef < 6; acoef++) {
      b_obj_BinaryLearners += fmax(0.0, 1.0 - M[acoef + 6 * k] * pscore[acoef]);
      bcoef++;
    }

    if (bcoef == 0) {
      b_obj_BinaryLearners = 0.0;
    } else {
      b_obj_BinaryLearners /= (double)bcoef;
    }

    negloss[k] = -(b_obj_BinaryLearners / 2.0);
  }

  b_obj_BinaryLearners = negloss[0];
  bcoef = -1;
  if (negloss[0] < negloss[1]) {
    b_obj_BinaryLearners = negloss[1];
    bcoef = 0;
  }

  if (b_obj_BinaryLearners < negloss[2]) {
    b_obj_BinaryLearners = negloss[2];
    bcoef = 1;
  }

  if (b_obj_BinaryLearners < negloss[3]) {
    b_obj_BinaryLearners = negloss[3];
    bcoef = 2;
  }
  if(b_obj_BinaryLearners < -0.5) return 0.0;
  return obj_ClassNames[bcoef + 1];
}

/*
 * File trailer for CompactClassificationECOC.c
 *
 * [EOF]
 */

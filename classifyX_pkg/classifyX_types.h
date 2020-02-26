/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: classifyX_types.h
 *
 * MATLAB Coder version            : 4.3
 * C/C++ source code generated on  : 23-Jan-2020 12:13:50
 */

#ifndef CLASSIFYX_TYPES_H
#define CLASSIFYX_TYPES_H

/* Include Files */
#include "rtwtypes.h"

/* Type Definitions */
#ifndef enum_c_classreg_learning_coderutils_
#define enum_c_classreg_learning_coderutils_

enum c_classreg_learning_coderutils_
{
  Logit = 0,                           /* Default value */
  Doublelogit,
  Invlogit,
  Ismax,
  Sign,
  Symmetric,
  Symmetricismax,
  Symmetriclogit,
  Identity
};

#endif                                 /*enum_c_classreg_learning_coderutils_*/

#ifndef typedef_c_classreg_learning_coderutils_
#define typedef_c_classreg_learning_coderutils_

typedef enum c_classreg_learning_coderutils_ c_classreg_learning_coderutils_;

#endif                                 /*typedef_c_classreg_learning_coderutils_*/

#ifndef enum_d_classreg_learning_coderutils_
#define enum_d_classreg_learning_coderutils_

enum d_classreg_learning_coderutils_
{
  linear = 1,                          /* Default value */
  gaussian = 2,
  rbf = 2,
  polynomial = 3
};

#endif                                 /*enum_d_classreg_learning_coderutils_*/

#ifndef typedef_d_classreg_learning_coderutils_
#define typedef_d_classreg_learning_coderutils_

typedef enum d_classreg_learning_coderutils_ d_classreg_learning_coderutils_;

#endif                                 /*typedef_d_classreg_learning_coderutils_*/

#ifndef struct_emxArray_real_T_13
#define struct_emxArray_real_T_13

struct emxArray_real_T_13
{
  double data[13];
  int size[1];
};

#endif                                 /*struct_emxArray_real_T_13*/

#ifndef typedef_emxArray_real_T_13
#define typedef_emxArray_real_T_13

typedef struct emxArray_real_T_13 emxArray_real_T_13;

#endif                                 /*typedef_emxArray_real_T_13*/

#ifndef struct_emxArray_real_T_13x12
#define struct_emxArray_real_T_13x12

struct emxArray_real_T_13x12
{
  double data[156];
  int size[2];
};

#endif                                 /*struct_emxArray_real_T_13x12*/

#ifndef typedef_emxArray_real_T_13x12
#define typedef_emxArray_real_T_13x12

typedef struct emxArray_real_T_13x12 emxArray_real_T_13x12;

#endif                                 /*typedef_emxArray_real_T_13x12*/

#ifndef struct_sLr6ihNx0sKy0j9kTquYw4F_tag
#define struct_sLr6ihNx0sKy0j9kTquYw4F_tag

struct sLr6ihNx0sKy0j9kTquYw4F_tag
{
  emxArray_real_T_13 Alpha;
  double Bias;
  emxArray_real_T_13x12 SupportVectorsT;
  double Scale;
  double Order;
  double Mu[12];
  double Sigma[12];
  d_classreg_learning_coderutils_ KernelFunction;
  double ClassNames[2];
  int ClassNamesLength[2];
  c_classreg_learning_coderutils_ ScoreTransform;
  double Prior[2];
  boolean_T ClassLogicalIndices[2];
  double Cost[4];
};

#endif                                 /*struct_sLr6ihNx0sKy0j9kTquYw4F_tag*/

#ifndef typedef_c_classreg_learning_coder_class
#define typedef_c_classreg_learning_coder_class

typedef struct sLr6ihNx0sKy0j9kTquYw4F_tag c_classreg_learning_coder_class;

#endif                                 /*typedef_c_classreg_learning_coder_class*/
#endif

/*
 * File trailer for classifyX_types.h
 *
 * [EOF]
 */

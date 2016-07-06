//
// File: y.cpp
//
// MATLAB Coder version            : 2.7
// C/C++ source code generated on  : 05-Jul-2016 16:59:34
//

#include "stdafx.h"
// Include Files
#include "rt_nonfinite.h"
#include "y.h"
#include "y_emxutil.h"

// Function Declarations
static void eml_lusolve(const double A_data[], emxArray_real_T *B);
static double eml_matlab_zlarfg(int n, double *alpha1, emxArray_real_T *x, int
  ix0);
static void eml_qrsolve(const emxArray_real_T *A, emxArray_real_T *B, double Y
  [11]);
static double eml_xnrm2(int n, const emxArray_real_T *x, int ix0);
static void eml_xscal(int n, double a, emxArray_real_T *x, int ix0);
static double rt_hypotd_snf(double u0, double u1);

// Function Definitions

//
// Arguments    : const double A_data[]
//                emxArray_real_T *B
// Return Type  : void
//
static void eml_lusolve(const double A_data[], emxArray_real_T *B)
{
  double b_A_data[121];
  signed char ipiv[11];
  int i2;
  int j;
  int c;
  int kAcol;
  int ix;
  double temp;
  int k;
  double s;
  int jy;
  int ijA;
  memcpy(&b_A_data[0], &A_data[0], 121U * sizeof(double));
  for (i2 = 0; i2 < 11; i2++) {
    ipiv[i2] = (signed char)(1 + i2);
  }

  for (j = 0; j < 10; j++) {
    c = j * 12;
    kAcol = 0;
    ix = c;
    temp = fabs(b_A_data[c]);
    for (k = 2; k <= 11 - j; k++) {
      ix++;
      s = fabs(b_A_data[ix]);
      if (s > temp) {
        kAcol = k - 1;
        temp = s;
      }
    }

    if (b_A_data[c + kAcol] != 0.0) {
      if (kAcol != 0) {
        ipiv[j] = (signed char)((j + kAcol) + 1);
        ix = j;
        kAcol += j;
        for (k = 0; k < 11; k++) {
          temp = b_A_data[ix];
          b_A_data[ix] = b_A_data[kAcol];
          b_A_data[kAcol] = temp;
          ix += 11;
          kAcol += 11;
        }
      }

      i2 = (c - j) + 11;
      for (jy = c + 1; jy + 1 <= i2; jy++) {
        b_A_data[jy] /= b_A_data[c];
      }
    }

    kAcol = c;
    jy = c + 11;
    for (k = 1; k <= 10 - j; k++) {
      temp = b_A_data[jy];
      if (b_A_data[jy] != 0.0) {
        ix = c + 1;
        i2 = (kAcol - j) + 22;
        for (ijA = 12 + kAcol; ijA + 1 <= i2; ijA++) {
          b_A_data[ijA] += b_A_data[ix] * -temp;
          ix++;
        }
      }

      jy += 11;
      kAcol += 11;
    }
  }

  for (kAcol = 0; kAcol < 10; kAcol++) {
    if (ipiv[kAcol] != kAcol + 1) {
      temp = B->data[kAcol];
      B->data[kAcol] = B->data[ipiv[kAcol] - 1];
      B->data[ipiv[kAcol] - 1] = temp;
    }
  }

  for (k = 0; k < 11; k++) {
    kAcol = 11 * k;
    if (B->data[k] != 0.0) {
      for (jy = k + 1; jy + 1 < 12; jy++) {
        B->data[jy] -= B->data[k] * b_A_data[jy + kAcol];
      }
    }
  }

  for (k = 10; k > -1; k += -1) {
    kAcol = 11 * k;
    if (B->data[k] != 0.0) {
      B->data[k] /= b_A_data[k + kAcol];
      for (jy = 0; jy + 1 <= k; jy++) {
        B->data[jy] -= B->data[k] * b_A_data[jy + kAcol];
      }
    }
  }
}

//
// Arguments    : int n
//                double *alpha1
//                emxArray_real_T *x
//                int ix0
// Return Type  : double
//
static double eml_matlab_zlarfg(int n, double *alpha1, emxArray_real_T *x, int
  ix0)
{
  double tau;
  double xnorm;
  int knt;
  int k;
  tau = 0.0;
  if (n <= 0) {
  } else {
    xnorm = eml_xnrm2(n - 1, x, ix0);
    if (xnorm != 0.0) {
      xnorm = rt_hypotd_snf(*alpha1, xnorm);
      if (*alpha1 >= 0.0) {
        xnorm = -xnorm;
      }

      if (fabs(xnorm) < 1.0020841800044864E-292) {
        knt = 0;
        do {
          knt++;
          eml_xscal(n - 1, 9.9792015476736E+291, x, ix0);
          xnorm *= 9.9792015476736E+291;
          *alpha1 *= 9.9792015476736E+291;
        } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

        xnorm = eml_xnrm2(n - 1, x, ix0);
        xnorm = rt_hypotd_snf(*alpha1, xnorm);
        if (*alpha1 >= 0.0) {
          xnorm = -xnorm;
        }

        tau = (xnorm - *alpha1) / xnorm;
        eml_xscal(n - 1, 1.0 / (*alpha1 - xnorm), x, ix0);
        for (k = 1; k <= knt; k++) {
          xnorm *= 1.0020841800044864E-292;
        }

        *alpha1 = xnorm;
      } else {
        tau = (xnorm - *alpha1) / xnorm;
        eml_xscal(n - 1, 1.0 / (*alpha1 - xnorm), x, ix0);
        *alpha1 = xnorm;
      }
    }
  }

  return tau;
}

//
// Arguments    : const emxArray_real_T *A
//                emxArray_real_T *B
//                double Y[11]
// Return Type  : void
//
static void eml_qrsolve(const emxArray_real_T *A, emxArray_real_T *B, double Y
  [11])
{
  int m;
  int mn;
  emxArray_real_T *b_A;
  int i1;
  int itemp;
  int b_m;
  int b_mn;
  double tau_data[11];
  signed char jpvt[11];
  double work[11];
  double vn1[11];
  double vn2[11];
  int k;
  int iy;
  double smax;
  double atmp;
  int pvt;
  double absxk;
  double t;
  int i;
  int i_i;
  int mmi;
  int ix;
  int i_ip1;
  int lastv;
  int lastc;
  boolean_T exitg2;
  int32_T exitg1;
  unsigned int b_i;
  m = A->size[0] - 2;
  if (A->size[0] <= 11) {
    mn = A->size[0];
  } else {
    mn = 11;
  }

  b_emxInit_real_T(&b_A, 2);
  i1 = b_A->size[0] * b_A->size[1];
  b_A->size[0] = A->size[0];
  b_A->size[1] = 11;
  emxEnsureCapacity((emxArray__common *)b_A, i1, (int)sizeof(double));
  itemp = A->size[0] * A->size[1];
  for (i1 = 0; i1 < itemp; i1++) {
    b_A->data[i1] = A->data[i1];
  }

  b_m = A->size[0];
  if (A->size[0] <= 11) {
    b_mn = A->size[0];
  } else {
    b_mn = 11;
  }

  for (i1 = 0; i1 < 11; i1++) {
    jpvt[i1] = (signed char)(1 + i1);
  }

  if (A->size[0] == 0) {
  } else {
    memset(&work[0], 0, 11U * sizeof(double));
    k = 0;
    for (iy = 0; iy < 11; iy++) {
      smax = 0.0;
      if (b_m < 1) {
      } else if (b_m == 1) {
        smax = fabs(A->data[k]);
      } else {
        atmp = 2.2250738585072014E-308;
        pvt = k + b_m;
        for (itemp = k; itemp + 1 <= pvt; itemp++) {
          absxk = fabs(A->data[itemp]);
          if (absxk > atmp) {
            t = atmp / absxk;
            smax = 1.0 + smax * t * t;
            atmp = absxk;
          } else {
            t = absxk / atmp;
            smax += t * t;
          }
        }

        smax = atmp * sqrt(smax);
      }

      vn1[iy] = smax;
      vn2[iy] = vn1[iy];
      k += b_m;
    }

    for (i = 0; i + 1 <= b_mn; i++) {
      i_i = i + i * b_m;
      mmi = (b_m - i) - 1;
      itemp = 1;
      if (11 - i > 1) {
        ix = i;
        smax = vn1[i];
        for (k = 2; k <= 11 - i; k++) {
          ix++;
          if (vn1[ix] > smax) {
            itemp = k;
            smax = vn1[ix];
          }
        }
      }

      pvt = (i + itemp) - 1;
      if (pvt + 1 != i + 1) {
        ix = b_m * pvt;
        iy = b_m * i;
        for (k = 1; k <= b_m; k++) {
          smax = b_A->data[ix];
          b_A->data[ix] = b_A->data[iy];
          b_A->data[iy] = smax;
          ix++;
          iy++;
        }

        itemp = jpvt[pvt];
        jpvt[pvt] = jpvt[i];
        jpvt[i] = (signed char)itemp;
        vn1[pvt] = vn1[i];
        vn2[pvt] = vn2[i];
      }

      if (i + 1 < b_m) {
        atmp = b_A->data[i_i];
        tau_data[i] = eml_matlab_zlarfg(1 + mmi, &atmp, b_A, i_i + 2);
      } else {
        smax = b_A->data[i_i];
        atmp = b_A->data[i_i];
        b_A->data[i_i] = smax;
        tau_data[i] = 0.0;
      }

      b_A->data[i_i] = atmp;
      if (i + 1 < 11) {
        atmp = b_A->data[i_i];
        b_A->data[i_i] = 1.0;
        i_ip1 = (i + (i + 1) * b_m) + 1;
        if (tau_data[i] != 0.0) {
          lastv = mmi;
          itemp = i_i + mmi;
          while ((lastv + 1 > 0) && (b_A->data[itemp] == 0.0)) {
            lastv--;
            itemp--;
          }

          lastc = 10 - i;
          exitg2 = false;
          while ((!exitg2) && (lastc > 0)) {
            itemp = i_ip1 + (lastc - 1) * b_m;
            k = itemp;
            do {
              exitg1 = 0;
              if (k <= itemp + lastv) {
                if (b_A->data[k - 1] != 0.0) {
                  exitg1 = 1;
                } else {
                  k++;
                }
              } else {
                lastc--;
                exitg1 = 2;
              }
            } while (exitg1 == 0);

            if (exitg1 == 1) {
              exitg2 = true;
            }
          }
        } else {
          lastv = -1;
          lastc = 0;
        }

        if (lastv + 1 > 0) {
          if (lastc == 0) {
          } else {
            for (iy = 1; iy <= lastc; iy++) {
              work[iy - 1] = 0.0;
            }

            iy = 0;
            i1 = i_ip1 + b_m * (lastc - 1);
            itemp = i_ip1;
            while ((b_m > 0) && (itemp <= i1)) {
              ix = i_i;
              smax = 0.0;
              pvt = itemp + lastv;
              for (k = itemp; k <= pvt; k++) {
                smax += b_A->data[k - 1] * b_A->data[ix];
                ix++;
              }

              work[iy] += smax;
              iy++;
              itemp += b_m;
            }
          }

          if (-tau_data[i] == 0.0) {
          } else {
            itemp = 0;
            for (iy = 1; iy <= lastc; iy++) {
              if (work[itemp] != 0.0) {
                smax = work[itemp] * -tau_data[i];
                ix = i_i;
                i1 = lastv + i_ip1;
                for (pvt = i_ip1; pvt <= i1; pvt++) {
                  b_A->data[pvt - 1] += b_A->data[ix] * smax;
                  ix++;
                }
              }

              itemp++;
              i_ip1 += b_m;
            }
          }
        }

        b_A->data[i_i] = atmp;
      }

      for (iy = i + 1; iy + 1 < 12; iy++) {
        itemp = (i + b_m * iy) + 1;
        if (vn1[iy] != 0.0) {
          smax = fabs(b_A->data[i + b_A->size[0] * iy]) / vn1[iy];
          smax = 1.0 - smax * smax;
          if (smax < 0.0) {
            smax = 0.0;
          }

          atmp = vn1[iy] / vn2[iy];
          atmp = smax * (atmp * atmp);
          if (atmp <= 1.4901161193847656E-8) {
            if (i + 1 < b_m) {
              smax = 0.0;
              if (mmi < 1) {
              } else if (mmi == 1) {
                smax = fabs(b_A->data[itemp]);
              } else {
                atmp = 2.2250738585072014E-308;
                pvt = itemp + mmi;
                while (itemp + 1 <= pvt) {
                  absxk = fabs(b_A->data[itemp]);
                  if (absxk > atmp) {
                    t = atmp / absxk;
                    smax = 1.0 + smax * t * t;
                    atmp = absxk;
                  } else {
                    t = absxk / atmp;
                    smax += t * t;
                  }

                  itemp++;
                }

                smax = atmp * sqrt(smax);
              }

              vn1[iy] = smax;
              vn2[iy] = vn1[iy];
            } else {
              vn1[iy] = 0.0;
              vn2[iy] = 0.0;
            }
          } else {
            vn1[iy] *= sqrt(smax);
          }
        }
      }
    }
  }

  atmp = 0.0;
  if (mn > 0) {
    if (A->size[0] >= 11) {
      itemp = A->size[0];
    } else {
      itemp = 11;
    }

    smax = (double)itemp * fabs(b_A->data[0]) * 2.2204460492503131E-16;
    k = 0;
    while ((k <= mn - 1) && (!(fabs(b_A->data[k + b_A->size[0] * k]) <= smax)))
    {
      atmp++;
      k++;
    }
  }

  memset(&Y[0], 0, 11U * sizeof(double));
  for (iy = 0; iy < mn; iy++) {
    if (tau_data[iy] != 0.0) {
      smax = B->data[iy];
      i1 = m - iy;
      for (i = 0; i <= i1; i++) {
        b_i = ((unsigned int)iy + i) + 2U;
        smax += b_A->data[((int)b_i + b_A->size[0] * iy) - 1] * B->data[(int)b_i
          - 1];
      }

      smax *= tau_data[iy];
      if (smax != 0.0) {
        B->data[iy] -= smax;
        i1 = m - iy;
        for (i = 0; i <= i1; i++) {
          b_i = ((unsigned int)iy + i) + 2U;
          B->data[(int)b_i - 1] -= b_A->data[((int)b_i + b_A->size[0] * iy) - 1]
            * smax;
        }
      }
    }
  }

  for (i = 0; i < (int)atmp; i++) {
    Y[jpvt[i] - 1] = B->data[i];
  }

  for (iy = 0; iy < (int)-(1.0 + (-1.0 - atmp)); iy++) {
    smax = atmp + -(double)iy;
    Y[jpvt[(int)smax - 1] - 1] /= b_A->data[((int)smax + b_A->size[0] * ((int)
      smax - 1)) - 1];
    for (i = 0; i <= (int)smax - 2; i++) {
      Y[jpvt[i] - 1] -= Y[jpvt[(int)smax - 1] - 1] * b_A->data[i + b_A->size[0] *
        ((int)smax - 1)];
    }
  }

  emxFree_real_T(&b_A);
}

//
// Arguments    : int n
//                const emxArray_real_T *x
//                int ix0
// Return Type  : double
//
static double eml_xnrm2(int n, const emxArray_real_T *x, int ix0)
{
  double b_y;
  double scale;
  int kend;
  int k;
  double absxk;
  double t;
  b_y = 0.0;
  if (n < 1) {
  } else if (n == 1) {
    b_y = fabs(x->data[ix0 - 1]);
  } else {
    scale = 2.2250738585072014E-308;
    kend = (ix0 + n) - 1;
    for (k = ix0; k <= kend; k++) {
      absxk = fabs(x->data[k - 1]);
      if (absxk > scale) {
        t = scale / absxk;
        b_y = 1.0 + b_y * t * t;
        scale = absxk;
      } else {
        t = absxk / scale;
        b_y += t * t;
      }
    }

    b_y = scale * sqrt(b_y);
  }

  return b_y;
}

//
// Arguments    : int n
//                double a
//                emxArray_real_T *x
//                int ix0
// Return Type  : void
//
static void eml_xscal(int n, double a, emxArray_real_T *x, int ix0)
{
  int i3;
  int k;
  i3 = (ix0 + n) - 1;
  for (k = ix0; k <= i3; k++) {
    x->data[k - 1] *= a;
  }
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_hypotd_snf(double u0, double u1)
{
  double b_y;
  double a;
  double b;
  a = fabs(u0);
  b = fabs(u1);
  if (a < b) {
    a /= b;
    b_y = b * sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    b_y = a * sqrt(b * b + 1.0);
  } else if (rtIsNaN(b)) {
    b_y = b;
  } else {
    b_y = a * 1.4142135623730951;
  }

  return b_y;
}

//
// Arguments    : const emxArray_real_T *a
//                const emxArray_real_T *b
//                emxArray_real_T *b_y
// Return Type  : void
//
void y(const emxArray_real_T *a, const emxArray_real_T *b, emxArray_real_T *b_y)
{
  emxArray_real_T *b_b;
  int i0;
  int loop_ub;
  double dv0[11];
  emxInit_real_T(&b_b, 1);
  if ((a->size[0] == 0) || (b->size[0] == 0)) {
    i0 = b_y->size[0];
    b_y->size[0] = 11;
    emxEnsureCapacity((emxArray__common *)b_y, i0, (int)sizeof(double));
    for (i0 = 0; i0 < 11; i0++) {
      b_y->data[i0] = 0.0;
    }
  } else if (a->size[0] == 11) {
    i0 = b_y->size[0];
    b_y->size[0] = b->size[0];
    emxEnsureCapacity((emxArray__common *)b_y, i0, (int)sizeof(double));
    loop_ub = b->size[0];
    for (i0 = 0; i0 < loop_ub; i0++) {
      b_y->data[i0] = b->data[i0];
    }

    eml_lusolve(a->data, b_y);
  } else {
    i0 = b_b->size[0];
    b_b->size[0] = b->size[0];
    emxEnsureCapacity((emxArray__common *)b_b, i0, (int)sizeof(double));
    loop_ub = b->size[0];
    for (i0 = 0; i0 < loop_ub; i0++) {
      b_b->data[i0] = b->data[i0];
    }

    eml_qrsolve(a, b_b, dv0);
    i0 = b_y->size[0];
    b_y->size[0] = 11;
    emxEnsureCapacity((emxArray__common *)b_y, i0, (int)sizeof(double));
    for (i0 = 0; i0 < 11; i0++) {
      b_y->data[i0] = dv0[i0];
    }
  }

  emxFree_real_T(&b_b);
}

//
// File trailer for y.cpp
//
// [EOF]
//

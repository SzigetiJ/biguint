/*******************************************************
 Approximation of PI using Taylor series of sin(x).

 Input:
  <argv[1]>: n: precision.
 Output:
  stderr: PI approximations
  stdout: final PI approximation
 Limits: works as expected if prec is less
 than the half of decimal digit capacity of BigUInt128
 (i.e., 18).
 ******************************************************/
#include "bigdecimal128.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Writes bigdecimal to out with accompanying message
void log_bdec(FILE *out, const BigDecimal128 *a, const char *txt) {
 char buf[2 * 128 + 4];
 buf[bigdecimal128_print(a, buf, 2 * 128 + 3)] = 0;
 fprintf(out, "%s: %s\n", txt, buf);
}

// In this multiplication the product overflow is handled in a simple way:
// as long as the result does not fit into the storage type,
// the greater factor is divided by 10, and this division is denoted in
// the eprec output parameter (it gets decreased).
// So finally, a * b * 0.1^eprec(in) ~= retv * 0.1^eprec(out) will be true.
BigUInt128 buint_mul_simple(const BigUInt128 *a, const BigUInt128 *b, UInt *eprec) {
 BigUInt128 av = *a;
 BigUInt128 bv = *b;
 while (true) {
  BigUIntPair128 pow_tmp = biguint128_dmul(&av, &bv);
  if (!biguint128_eqz(&pow_tmp.second) || bigint128_ltz(&pow_tmp.first)) {
   --*eprec;
   if (biguint128_lt(&av, &bv)) {
    bv = biguint128_div10(&bv).first;
   } else {
    av = biguint128_div10(&av).first;
   }
  } else {
   av = pow_tmp.first;
   break;
  }
 }
 return av;
}

// Calculates factorial incrementally (taking (n-k)! as input for getting n!).
// If the result can be divided by 10, it is divided indeed, and this division is
// denoted by increasing the output parameter eprec.
BigUInt128 extend_factorial(const BigUInt128 *base, unsigned int base_n, unsigned int dst_n, UInt *eprec) {
 BigUInt128 a = biguint128_value_of_uint(base_n + 1);
 BigUInt128 retv = *base;
 for (unsigned int i = base_n; i < dst_n; ++i) {
  retv = buint_mul_simple(&retv, &a, eprec);
  biguint128_inc(&a);
  if ((i + 1) % 5 == 0) {
   retv = biguint128_div10(&retv).first;
   ++*eprec;
  }
  if ((i + 1) % 25 == 0) {
   retv = biguint128_div10(&retv).first;
   ++*eprec;
  }
 }
 return retv;
}

// Approximates sin(x) with it Taylor series to dst_prec digits precision.
BigDecimal128 sin_series(const BigDecimal128 *x, UInt dst_prec) {
 BigUInt128 one = biguint128_ctor_unit();
 // this variable accumulates the Taylor serie members
 BigDecimal128 retv = {biguint128_ctor_default(), dst_prec};

 // x^2
 UInt x2_prec = 2 * x->prec;
 BigUInt128 x2_val = buint_mul_simple(&x->val, &x->val, &x2_prec);

 // xi variable is calculated as xi_num / xi_denom
 // both xi_num and xi_denom are defined incrementally
 BigDecimal128 xi_num = *x;
 BigDecimal128 xi_denom = {one, 0};

 // In the series, the xi members are added/subtracted alternately.
 // First, x1 is added to the sum.
 bool add = true;
 unsigned int i = 1;
 while (true) {
  BigDecimal128 xi;
  buint_bool div_res = bigdecimal128_div_safe(&xi, &xi_num, &xi_denom, dst_prec);
  if (!div_res) {
   fprintf(stderr, "DIV overflow\n");
  }
  if (biguint128_eqz(&xi.val)) {
   break;
  }
  if (add) {
   retv = bigdecimal128_add(&retv, &xi);
  } else {
   retv = bigdecimal128_sub(&retv, &xi);
  }
  add = !add;

  xi_num.prec += x2_prec;
  xi_num.val = buint_mul_simple(&xi_num.val, &x2_val, &xi_num.prec);
  xi_denom.val = extend_factorial(&xi_denom.val, i, i + 2, &xi_num.prec);
  i += 2;
 }
 return retv;
}

int main(int argc, const char *argv[]) {
 if (argc < 2) {
  fprintf(stderr, "Usage:\n\t%s <precision>\n", argv[0]);
  return 1;
 }

 // calculation precision must be somewhat higher than the result precision
 unsigned int prec = atoi(argv[1]) + 1;

 // initial hint: PI is in interval [3.1, 3.2]
 BigDecimal128 x_lo = bigdecimal128_ctor_precv((BigDecimal128){biguint128_value_of_uint(31), 1}, prec);
 BigDecimal128 x_hi = bigdecimal128_ctor_precv((BigDecimal128){biguint128_value_of_uint(32), 1}, prec);

 while (true) {
  BigDecimal128 x_mid = (BigDecimal128){biguint128_shrv(biguint128_add(&x_lo.val, &x_hi.val), 1), prec};
  if (biguint128_eq(&x_mid.val, &x_lo.val)) break;
  BigDecimal128 sinx_mid = sin_series(&x_mid, prec);
  log_bdec(stderr, &x_mid, "PI (aprx)");
  log_bdec(stderr, &sinx_mid, "sin(x)");
  if (bigint128_ltz(&sinx_mid.val)) {
   x_hi = x_mid;
  } else {
   x_lo = x_mid;
  }
 }

 BigDecimal128 x_res = bigdecimal128_ctor_prec(&x_lo, prec - 1);
 log_bdec(stdout, &x_res, "PI");

 return 0;
}

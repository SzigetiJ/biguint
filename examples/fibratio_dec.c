/**********************************
 This example shows the difference
 between normal and fast BigDecimal
 division: the fast version fails
 sooner as the numbers get large or
 the precision is high.
 Input:
  -
 Output:
  stdout:
   - Fibonacci-numbers and their ratio
   obtained by using different div()
   functions.
 Limits:
  -
**********************************/

#include <stdio.h>
#include <string.h>
#include "bigdecimal384.h"

#define BUINT_DIGITS_CAP (((384 - 1) * 3) / 10)	// as 10^3 ~= 2^10
#define BUFSIZE (BUINT_DIGITS_CAP + 3)	// +3: '+'/'-' sign, leading zero, decimal dot before the value 
#define ITERATIONS ((478 * BUINT_DIGITS_CAP) / 100)	// as log(10) / log((sqrt(5)+1)/2) ~= 4.78


int main(int argc, const char *argv[]) {
 // beginning with Fib(0)=1, Fib(1)=1
 BigDecimal384 a = {biguint384_ctor_unit(),0};
 BigDecimal384 b = {biguint384_ctor_unit(),0};
 // this variable will store the last a/b ratio (Fib(i-1)/Fib(i-2)) for comparison
 BigDecimal384 last_r2 = bigdecimal384_ctor_default();
 // char buffers for printing
 char bufa[BUFSIZE+1];
 char bufb[BUFSIZE+1];
 char bufr1[BUFSIZE+1];
 char bufr2[BUFSIZE+1];

 for (unsigned int i = 0; i < ITERATIONS; ++i) {
  // evolving Fib(i):=Fib(i-1)+Fib(i-2)
  BigDecimal384 tmp = bigdecimal384_add(&a, &b);
  b = a;
  a = tmp;

  // approximating golden ratio
  // first, with fast division (with lower precision)
  BigDecimal384 r_v1 = bigdecimal384_div_fast(&a, &b, BUINT_DIGITS_CAP/2 - 1);
  // next, with "normal" division
  BigDecimal384 r_v2 = bigdecimal384_div(&a, &b, BUINT_DIGITS_CAP - 1);

  // printing
  bufa[bigdecimal384_print(&a, bufa, BUFSIZE)] = 0;
  bufb[bigdecimal384_print(&b, bufb, BUFSIZE)] = 0;
  bufr1[bigdecimal384_print(&r_v1, bufr1, BUFSIZE)] = 0;
  bufr2[bigdecimal384_print(&r_v2, bufr2, BUFSIZE)] = 0;

  printf("Iteration #%u\na: %s\nb: %s\n", i, bufa, bufb);
  printf(" golden ratio apprx.v1: %s\n", bufr1);
  printf(" golden ratio apprx.v2: %s (change %c)\n",
    bufr2,
    bigdecimal384_lt(&last_r2,&r_v2) ? '+':
    bigdecimal384_eq(&last_r2,&r_v2) ? '=':
    '-');

  // storing result
  last_r2 = r_v2;
 }

 return 0;
}


/**********************************
 Approximation to golden ratio by
 dividing neighbors of Fibonacci
 sequence.
 
 Input:
  <argv[1]>: Number of iterations
 Output:
  stdout: in each iteration step
  a line is printed containing
  i and (F(i+1)*PRECISION) / F(i)
 Limits:
  184 iterations give valid results.
**********************************/
#include "biguint384.h"
#include <stdlib.h>
#include <stdio.h>

#define PRECISION 77
#define STR_BUF_SIZE ((384 * 90) / 299 + 2)

int main(int argc, const char *argv[]) {
 BigUInt384 a = biguint384_ctor_unit();
 BigUInt384 b = biguint384_ctor_copy(&a);
 BigUInt384 mul;

 char str_buf[STR_BUF_SIZE];
 int steps;
 
 if (1 < argc) {
  steps = atoi(argv[1]);
 } else {
  fprintf(stderr,"Usage:\n\t%s <steps>\n", argv[0]);
  return 1;
 }

 str_buf[0]='1';
 for (int i=0; i<PRECISION; ++i) {
  str_buf[i+1] = '0';
 }
 mul = biguint384_ctor_deccstream(str_buf, PRECISION + 1);

 for (int i=1; i<=steps; ++i) {
  BigUInt384 sum = biguint384_add(&a, &b);
  b = a;
  a = sum;

  BigUInt384 a_mul = biguint384_mul(&a, &mul);
  BigUInt384 ratio_mul = biguint384_div(&a_mul, &b).first;

  str_buf[biguint384_print_dec(&ratio_mul, str_buf, STR_BUF_SIZE)]=0;
  printf("%3d: %s\n", i, str_buf);
 }

 return 0;
}

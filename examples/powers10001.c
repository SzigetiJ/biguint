/**********************************
 This example enumerates the powers
 of 10001. The results are arranged
 vertically centered, so that one
 can observe the pattern of the
 Pascal triangle. At least for the
 first 15 powers.
 
 Input:
  <argv[1]>: Number of iterations
 Output:
  stdout: in each iteration step
  a line is printed containing
  i (left justified) and 10001^i
  (centered). The 0th power (1) is
  suppressed.
**********************************/
#include "biguint256.h"
#include <stdlib.h>
#include <stdio.h>

#define BASE "10001"
#define BASE_LEN (sizeof(BASE)-1)/sizeof(char)

#define STR_BUF_SIZE 90

int main(int argc, const char *argv[]) {
 BigUInt256 base = biguint256_ctor_deccstream(BASE, BASE_LEN);
 BigUInt256 prod = biguint256_ctor_unit();
 char str_buf[STR_BUF_SIZE];
 int max_power;
 
 if (1 < argc) {
  max_power = atoi(argv[1]);
 } else {
  fprintf(stderr,"Usage:\n\t%s <max_power>\n", argv[0]);
  return 1;
 }

 for (int i=1; i<=max_power; ++i) {
  prod = biguint256_mul(&base, &prod);
  str_buf[biguint256_print_dec(&prod, str_buf, STR_BUF_SIZE)]=0;
  printf("%2d: %*s%s\n", i, 2*(max_power-i), "", str_buf);
 }

 return 0;
}

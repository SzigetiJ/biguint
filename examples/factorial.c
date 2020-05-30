/**********************************
 This example demonstrates factorial
 calculation.

 Input:
  <argv[1]>: N (unsigned integer)
  
 Output:
  stdout: N lines where line i
  contains line index i and i!.

 Valid input interval: [1,57]
**********************************/
#include "biguint256.h"
#include <stdlib.h>
#include <stdio.h>

#define STR_BUF_SIZE 90

int main(int argc, const char *argv[]) {
 BigUInt256 unit = biguint256_ctor_unit();
 BigUInt256 factor = unit;	// bit-level copy
 BigUInt256 prod = unit;	// bit-level copy
 char str_buf[STR_BUF_SIZE];
 unsigned int N;
 
 if (1 < argc) {
  N = atoi(argv[1]);
 } else {
  fprintf(stderr,"Usage:\n\t%s <N>\n", argv[0]);
  return 1;
 }

 for (unsigned int i=1; i<=N; ++i) {
  prod = biguint256_mul(&prod, &factor);
  factor = biguint256_add(&factor, &unit);
  str_buf[biguint256_print_dec(&prod, str_buf, STR_BUF_SIZE)]=0;
  printf("%3d: %*s\n", i, 72, str_buf);
 }

 return 0;
}

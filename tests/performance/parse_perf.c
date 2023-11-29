/***************************************
 This example performs multiple (CYCLES)
 string parsing (and summing the parsed
 values).
 The main purpose of this program is to
 check the performance of the parser
 function (biguint512_ctor_deccstream)
 and its components like multiplication
 by 10 and addition.
 Input:
  -
 Output:
  stdout: The sum of the parsed values.
***************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biguint512.h"

#define CYCLES 10000

#define DECDIGITS (3*512/10)	// Approx. max. usage of 512 bits

int main(int argc, const char *argv[]) {
 BigUInt512 sum =  biguint512_ctor_default();
 char sum_str[DECDIGITS + 10];

 char num_str[DECDIGITS + 1];
 // initialize the num as "111..1"
 memset(num_str, '1', DECDIGITS);
 num_str[DECDIGITS] = 0;

 for (unsigned int i = 0U; i < CYCLES; ++i) {
  num_str[i % DECDIGITS] ='0' + ((i * 7) % 10);	// make some change on the string to parse

  // call the parser function
  BigUInt512 num = biguint512_ctor_deccstream(num_str, DECDIGITS);

  // make something with the parsed value
  sum = biguint512_add(&sum, &num);
 }

 // finally, print the result (sum)
 sum_str[biguint512_print_dec(&sum, sum_str, DECDIGITS + 10)] = 0; 
 printf("sum: %s\n", sum_str);

 return 0;
}


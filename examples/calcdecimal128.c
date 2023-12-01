/**********************************
 This example shows how to use
 BigDecimal types for arithmetic
 calculations.
 Input:
  cmdline:
   argv[1]: arithmetic operation
    character.
   argv[2]: precision of the result
   argv[3] and argv[4]: big decimal
    numbers.
 Output:
  stdout:
   result of the arithmetic function.
 Limits:
  TODO reference on BigDecimal
  operating range.
**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigdecimal128.h"

#define BUINT_DIGITS_CAP (((128 - 1) * 3) / 10)	// as 10^3 ~= 2^10
#define BUFSIZE (BUINT_DIGITS_CAP + 3)	// +3: '+'/'-' sign, leading zero, decimal dot before the value 

int main(int argc, const char *argv[]) {
 if (argc < 5) {
  fprintf(stderr, "Usage: %s <function> <prec> <operand1> <operand2>\n", argv[0]);
  fprintf(stderr, " where function is one of 'add', 'sub', 'mul' or 'div',\n");
  fprintf(stderr, " prec is the precision of the result,\n");
  fprintf(stderr, " and operand1, operand2 are arbitrary decimal numbers that fit into BigDecimal storage type\n");
  return 1;
 }

 // parameter processing
 BigDecimal128 (*fun)(const BigDecimal128 *a, const BigDecimal128 *b) =
  (strcmp("add", argv[1]) == 0)? bigdecimal128_add:
  (strcmp("sub", argv[1]) == 0)? bigdecimal128_sub:
  (strcmp("mul", argv[1]) == 0)? bigdecimal128_mul:
  NULL;
 buint_bool fun_is_div = (strcmp("div", argv[1]) == 0);
 UInt prec = atoi(argv[2]);
 BigDecimal128 a = bigdecimal128_ctor_cstream(argv[3],strlen(argv[3]));
 BigDecimal128 b = bigdecimal128_ctor_cstream(argv[4],strlen(argv[4]));

 // parameter check
 if (!fun_is_div && fun == NULL) {
  fprintf(stderr, "Invalid function: [%s]\n", argv[1]);
  return 1;
 }

 // do the math
 BigDecimal128 res = fun_is_div?
  bigdecimal128_div(&a, &b, prec):
  fun(&a, &b);
 if (!fun_is_div) {
  res = bigdecimal128_ctor_prec(&res, prec);
 }

 // output
 char buf[BUFSIZE];
 buf[bigdecimal128_print(&res,buf,BUFSIZE)] = 0;
 printf("%s\n", buf);

 return 0;
}


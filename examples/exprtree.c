/**********************************
 How to construct an arithmetic
 expression tree with biguint
 values and functions
 Input:
  -
 Output:
  stderr: some internal information.
 Limits:
  -
**********************************/
#include <stdio.h>
#include <string.h>
#include "biguint128.h"

#define BUFLEN 40

// expression evaluation is an explicit sequence of function evaluations
BigUInt128 fun_2a_plus_b_funseq(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 tmp=biguint128_shl(a,1);
 BigUInt128 res=biguint128_add(&tmp, b);
 return res;
}

BigUInt128 fun_2a_plus_b_paramtree(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 tmp=biguint128_ctor_default();
 return biguint128_add(
         biguint128_shl_or(&tmp,a,1),
         b
        );
}

int main() {
 const char a_str[]="1000000000000000000000000"; // 10^24
 const char b_str[]="10000000000000000000000000"; // 10^25
 char sep_str[BUFLEN+1];
 char res_str[BUFLEN];
 BigUInt128 a = biguint128_ctor_deccstream(a_str, sizeof(a_str)/sizeof(a_str[0])-1);
 BigUInt128 b = biguint128_ctor_deccstream(b_str, sizeof(b_str)/sizeof(a_str[0])-1);
 memset(sep_str,'-',BUFLEN);
 sep_str[BUFLEN]=0;

 BigUInt128 res0 = fun_2a_plus_b_funseq(&a, &b);
 res_str[biguint128_print_dec(&res0, res_str, BUFLEN)]=0;
 fprintf(stderr, "a:   %*s\nb:   %*s\nfunc: 2*a+b\n-----%*s\nres: %*s\n",
         BUFLEN, a_str,
         BUFLEN, b_str,
         BUFLEN, sep_str,
         BUFLEN, res_str
        );
 return 0;
}

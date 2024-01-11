#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"

const CStr dec_samples[] = {
 STR("0"),
 STR("1"),
 STR("9"),
 STR("10"),
 STR("256"),
 STR("4444"),
 STR("65536"),
 STR("666666"),
 STR("7777777"),
 STR("88888888"),
 STR("999999999"),
 STR("1234567890"),
 STR("101"),
 STR("2002"),
 STR("30003"),
 STR("400004"),
 STR("5000005"),
 STR("60000006"),
 STR("700000007"),
 STR("1000000000"),
 STR("2000000000"),
 STR("3000000000"),
 STR("4000000000"),
 STR("4294967295"),
 STR("4294967296"),
 STR("4294967297"),
 STR("8000000008"),
 STR("90000000009"),
 STR("323456789012345678901234567890123456789"),
 STR("113456789012345678901234567890123456789012345678901234567890123456789012345678"),
 STR("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234")
};
const unsigned int dec_sample_len = ARRAYSIZE(dec_samples);

static buint_bool not_zero_(const GenArgU *args, unsigned int n) {
 return strcmp(args[1].str, "-0") != 0;
}

int main() {

 const unsigned int DSAMPLE_WIDTH = 8U;
 const unsigned int SIZE_LEN = 5U;
 char retvok[dec_sample_len][SIZE_LEN];
 char buflen[dec_sample_len][SIZE_LEN];
 char buflenprecise[dec_sample_len][SIZE_LEN];
 char buflenshort[dec_sample_len][SIZE_LEN];
 char retv0[dec_sample_len][SIZE_LEN];
 char neg[dec_sample_len][DEC_BIGINTLEN_HI];
 char retvokneg[dec_sample_len][SIZE_LEN];
 CStr dsamples[dec_sample_len][DSAMPLE_WIDTH];

 for (unsigned int i = 0; i < dec_sample_len; ++i) {
  snprintf(buflen[i], SIZE_LEN, "%u", (unsigned int) dec_samples[i].len + 1);
  snprintf(buflenprecise[i], SIZE_LEN, "%u", (unsigned int) dec_samples[i].len);
  snprintf(buflenshort[i], SIZE_LEN, "%u", (unsigned int) dec_samples[i].len - 1);
  snprintf(retvok[i], SIZE_LEN, "%u", (unsigned int) dec_samples[i].len);
  snprintf(retvokneg[i], SIZE_LEN, "%u", (unsigned int) dec_samples[i].len + 1);
  snprintf(retv0[i], SIZE_LEN, "0");
  unsigned int neglen = snprintf(neg[i], DEC_BIGINTLEN_HI, "-%s", dec_samples[i].str);
  dsamples[i][0] = dec_samples[i];
  dsamples[i][1].str = buflen[i];
  dsamples[i][1].len = SIZE_LEN;
  dsamples[i][2].str = buflenprecise[i];
  dsamples[i][2].len = SIZE_LEN;
  dsamples[i][3].str = buflenshort[i];
  dsamples[i][3].len = SIZE_LEN;
  dsamples[i][4].str = retvok[i];
  dsamples[i][4].len = SIZE_LEN;
  dsamples[i][5].str = retv0[i];
  dsamples[i][5].len = SIZE_LEN;
  dsamples[i][6].str = neg[i];
  dsamples[i][6].len = neglen;
  dsamples[i][7].str = retvokneg[i];
  dsamples[i][7].len = SIZE_LEN;
 }

 unsigned int pr_params[] = {0, 0, 1, 4};
 unsigned int prprec_params[] = {0, 0, 2, 4};
 unsigned int prpshort_params[] = {0, 0, 3, 5};
 unsigned int prneg_params[] = {6, 6, 1, 7};
 unsigned int prnegshort1_params[] = {6, 6, 2, 5};
 unsigned int prnegshort2_params[] = {6, 6, 3, 5};

 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_DEC, pr_params, XTFUN0(biguint128_print_dec), "print_dec", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_HEX, pr_params, XTFUN0(biguint128_print_hex), "print_hex", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, pr_params, XTFUN0(bigint128_print_dec), "print_sdec", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_DEC, prprec_params, XTFUN0(biguint128_print_dec), "print_dec", NULL) == 0);

 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_DEC, prpshort_params, XTFUN0(biguint128_print_dec), "print_dec", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_HEX, prpshort_params, XTFUN0(biguint128_print_hex), "print_hex", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, prpshort_params, XTFUN0(bigint128_print_dec), "print_sdec", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, prneg_params, XTFUN0(bigint128_print_dec), "print_sdec", not_zero_) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, prnegshort1_params, XTFUN0(bigint128_print_dec), "print_sdec", not_zero_) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, prnegshort2_params, XTFUN0(bigint128_print_dec), "print_sdec", not_zero_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_DEC, pr_params, XTFUN0V(biguint128_print_decv), "print_decv", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_HEX, pr_params, XTFUN0V(biguint128_print_hexv), "print_hexv", NULL) == 0);
 assert(test_genfun(&dsamples[0][0], DSAMPLE_WIDTH, dec_sample_len, FMT_SDEC, pr_params, XTFUN0V(bigint128_print_decv), "print_sdecv", NULL) == 0);
#endif
 return 0;
}

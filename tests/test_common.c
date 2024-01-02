#include <stdio.h>
#include <string.h>
#include "uint_types.h"
#include "test_common.h"

bool negate_strcpy(char *dest, size_t dest_size, const char *src, size_t src_len) {
 if (src[0]==NEG_SIGN) {
  if (src_len<=dest_size) {
   strcpy(dest,src+1);
   return true;
  }
 } else {
  if (strcmp(src,"0") == 0) {
   if (src_len<dest_size) {
    strcpy(dest,src);
    return true;
   }
  } else {
   if (src_len+1<dest_size) {
    dest[0]=NEG_SIGN;
    strcpy(dest+1,src);
    return true;
   }
  }
 }
 return false;
}

bool check_cstr(const char *fun, const CStr *expected, size_t actual_n, const char *actual) {
 bool pass = true;
 if (expected->len != actual_n) {
  fprintf(stderr, "%s length mismatch. Expected: [%" PRIbuint_size_t "], actual: [%" PRIbuint_size_t "]\n",
   fun, expected->len, actual_n);
  pass = false;
 } else if (0 != strcmp(expected->str, actual)) {
  fprintf(stderr, "%s data mismatch. Expected: [%.*s], actual: [%.*s]\n",
   fun, expected->len, expected->str, actual_n, actual);
  pass = false;
 }
 return pass;
}

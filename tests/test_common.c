#include <string.h>
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

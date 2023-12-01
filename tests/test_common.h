#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#define NEG_SIGN '-'

// auxiliary struct to eliminate strlen from the code
typedef struct {
 const char *str;
 size_t len;
} CStr;
#define STR(X) {X,sizeof(X)-1}

bool negate_strcpy(char *dest, size_t dest_size, const char *src, size_t src_len);

static inline const char *bool_to_str(int a) {
 return a?"TRUE":"FALSE";
}

#endif /* TEST_COMMON_H */


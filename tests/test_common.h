#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#define NEG_SIGN '-'

// parse / print format
typedef enum {FMT_HEX=0, FMT_DEC, FMT_SDEC} Format;

// auxiliary struct to eliminate strlen from the code
typedef struct {
 const char *str;
 size_t len;
} CStr;
#define STR(X) {X,sizeof(X)-1}
#define ARRAYSIZE(X) (sizeof(X)/sizeof(X[0]))

bool negate_strcpy(char *dest, size_t dest_size, const char *src, size_t src_len);

static inline const char *bool_to_str(int a) {
 return a?"TRUE":"FALSE";
}

static inline bool cstr_to_bool(const CStr *a) {
 return a->str[0] == '1' && a->len == 1;
}

bool check_cstr(const char *fun, const CStr *expected, size_t actual_n, const char *actual);

#endif /* TEST_COMMON_H */


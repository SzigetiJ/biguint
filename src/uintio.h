#ifndef _UINTIO_H_
#define _UINTIO_H_

typedef enum {
 INTIO_NO_ERROR = 0,
 INTIO_ERROR_OUTOFRANGE = 1
} IntIoError;

IntIoError get_digit(char, unsigned char, unsigned char*);
IntIoError get_hexbyte(const char*, unsigned char*);

void set_hexdigit(char*, unsigned char);
void set_hexbyte(char*, unsigned char);
void set_decdigit(char*, unsigned char);

#endif

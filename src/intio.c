/*****************************************************************************

    Copyright 2020 SZIGETI János

    This file is part of biguint library (Big Unsigned Integers).

    Biguint is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biguint is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*****************************************************************************/
#include "intio.h"

#define CHR_NUM_FIRST '0'
#define CHR_NUM_LAST '9'
#define CHR_ALPHA_FIRST_UC 'A'
#define CHR_ALPHA_LAST_UC 'Z'
#define CHR_ALPHA_FIRST_LC 'a'
#define CHR_ALPHA_LAST_LC 'z'
#define OFFSET_ALPHA 10

/**
 Interprets a single character as digit in the number system.
 \param a Character to interpret.
 \param base Number system base.
 \param result Pointer to write the result to.
 \return Error code.
*/
IntIoError get_digit(char a, unsigned char base, unsigned char *result) {
 if (CHR_NUM_FIRST <= a && a <= CHR_NUM_LAST) {
  *result = a - CHR_NUM_FIRST;
 } else if (CHR_ALPHA_FIRST_UC <= a && a <= CHR_ALPHA_LAST_UC) {
  *result = a - CHR_ALPHA_FIRST_UC + OFFSET_ALPHA;
 } else if (CHR_ALPHA_FIRST_LC <= a && a <= CHR_ALPHA_LAST_LC) {
  *result = a - CHR_ALPHA_FIRST_LC + OFFSET_ALPHA;
 } else {
  return INTIO_ERROR_OUTOFRANGE;
 }
 if (! (*result < base)) {
  return INTIO_ERROR_OUTOFRANGE;
 }
 return INTIO_NO_ERROR;
}

IntIoError get_hexbyte(const char *a, unsigned char *result) {
 unsigned char lo, hi;
 IntIoError err_hi = get_digit(a[0], 16, &hi);
 IntIoError err_lo = get_digit(a[1], 16, &lo);
 *result = hi<<4 | lo;
 return err_hi | err_lo;
}

void set_hexdigit(char *a, unsigned char value) {
 *a = value < OFFSET_ALPHA?
  (CHR_NUM_FIRST + value):
  (CHR_ALPHA_FIRST_UC + value - OFFSET_ALPHA);
}

void set_hexbyte(char *a, unsigned char value) {
 set_hexdigit(a,value>>4);
 set_hexdigit(a+1, value&0x0F);
}

void set_decdigit(char *a, unsigned char value) {
 set_hexdigit(a, value);
}


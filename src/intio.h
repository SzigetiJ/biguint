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
#ifndef _INTIO_H_
#define _INTIO_H_

/**
 Return values of basic methods.
*/
typedef enum {
 INTIO_NO_ERROR = 0,
 INTIO_ERROR_OUTOFRANGE = 1
} IntIoError;

/**
 @brief Reads a single digit.
 Interprets a character as a digit of a number.
 Values greater than 9 may be given either uppercase or lowercase.
 @param a Character to interpret as a digit.
 @param base Base of the digit.
 @param result The digit is stored here.
 @return INTIO_NO_ERROR: success, INTIO_ERROR_OUTOFRANGE: the given character cannot be interpreted in the given base.
*/
IntIoError get_digit(char a, unsigned char base, unsigned char *result);

/**
 @brief Reads two characters and interprets them as a hexadecimal number in range 0..255.
 @param a Points to first hex digit (big-endian). The second digit is at a+1.
 @param result Pointer to write the result to.
 @return Error code.
*/
IntIoError get_hexbyte(const char *a, unsigned char *result);

/**
 @brief Writes a value as a hexadecimal digit into a character.
 If the value is larger than 15, the outcome of the method is undefined.
 @param a Character to write to.
 @param value
*/

void set_hexdigit(char *a, unsigned char value);
/**
 @brief Writes a value as two hexadecimal digits into two consecutive characters (big-endian order).
 @param a Character array to write to.
 @param value
*/

void set_hexbyte(char *a, unsigned char value);
/**
 @brief Writes a value as a decimal digit into a character.
 If the value is larger than 9, the outcome of the method is undefined.
 @param a Character to write to.
 @param value
*/
void set_decdigit(char *a, unsigned char value);

#endif

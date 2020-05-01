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

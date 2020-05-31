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
#ifndef _UINT_TYPES_H_
#define _UINT_TYPES_H_

#ifdef USE_STD_TYPES
  #include <stddef.h>
  #include <stdbool.h>
  #include <stdint.h>

  typedef size_t buint_size_t;	///< Array length, bit/byte/digit index.
  typedef bool buint_bool;	///< Logical value.
  typedef uint32_t UInt;	///< Alias on the basic store unit.
#else
  typedef unsigned char buint_size_t;	///< Array length, bit/byte/digit index.
  typedef unsigned char buint_bool;	///< Logical value.
  typedef unsigned int UInt;	///< Alias on the basic store unit.
#endif  // USE_STD_TYPES

#endif  // _UINT_TYPES_H_



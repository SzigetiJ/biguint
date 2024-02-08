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
  #include <inttypes.h>

  typedef size_t buint_size_t;	///< Array length, bit/byte/digit index.
  typedef bool buint_bool;	///< Logical value.
  #ifdef USE_UINT64_T
    typedef uint64_t UInt;	///< Alias on the basic store unit.
    typedef uint32_t SUInt;	///< Half the size of UInt.
    #define HAVE_SUINT 1
    #define PRIuint PRIu64
    #define PRIuintX PRIX64
  #else
    typedef uint32_t UInt;	///< Alias on the basic store unit.
    #if (SIZEOF_UNSIGNED_LONG_LONG == 8 ||SIZEOF_UNSIGNED_LONG == 8)
      typedef uint64_t DUInt;	///< Double the size of UInt.
      #define HAVE_DUINT 1
    #endif
    typedef uint16_t SUInt;	///< Half the size of UInt.
    #define HAVE_SUINT 1
    #define PRIuint PRIu32
    #define PRIuintX PRIX32
  #endif
  #define PRIbuint_size_t "zu"
#else   // !USE_STD_TYPES
  typedef unsigned int buint_size_t;	///< Array length, bit/byte/digit index.
  typedef unsigned char buint_bool;	///< Logical value.
  typedef unsigned int UInt;	///< Alias on the basic store unit.
  #if (SIZEOF_UNSIGNED_LONG_LONG == 2 * SIZEOF_UNSIGNED_INT)
    typedef unsigned long long DUInt;
    #define HAVE_DUINT 1
  #elif (SIZEOF_UNSIGNED_LONG == 2 * SIZEOF_UNSIGNED_INT)
    typedef unsigned long DUInt;
    #define HAVE_DUINT 1
  #endif
  #if (SIZEOF_UNSIGNED_INT == 2 * SIZEOF_UNSIGNED_SHORT)
    typedef unsigned short SUInt;	///< Half the size of UInt.
    #define HAVE_SUINT 1
  #endif
  #define PRIuint "u"
  #define PRIuintX "X"
  #define PRIbuint_size_t "u"
#endif  // USE_STD_TYPES

typedef struct {
 buint_size_t byte_sel;
 buint_size_t bit_sel;
} buint_size_p;

#ifdef HAVE_DUINT
typedef DUInt wUInt;
typedef UInt nUInt;
#else
#ifdef HAVE_SUINT
typedef UInt wUInt;
typedef SUInt nUInt;
#else
#error No double-size or half-size integer type found for UInt.
#endif
#endif

#endif  // _UINT_TYPES_H_

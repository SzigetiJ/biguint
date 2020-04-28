# biguint
## Big Unsigned Integers

![C/C++ CI](https://github.com/SzigetiJ/biguint/workflows/C/C++%20CI/badge.svg)

Currently libbiguint provides the followin big unsigned integer types:
* `BigUInt128` (128 bits)
* `BigUInt256` (256 bits)

All the provided types are accompanied by the following functions:
* addition and substraction (add, sub);
* multiplication and division (mul, div/mod);
* bit shift operations (shl, shr);
* bitwise operations (and, or, xor, not);
* comparison (lt, eq);
* parsing and printing (from/to hex and dec strings);
* default and standard _constructors_ (initializer functions).

The source code of type `BigUInt128` is written in general manner.
The source of all other biguint types are generated codes derived from `BigUInt128`.

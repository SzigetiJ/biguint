## Big Unsigned Integers

![GitHub](https://img.shields.io/github/license/SzigetiJ/biguint)
![C/C++ CI](https://github.com/SzigetiJ/biguint/workflows/C/C++%20CI/badge.svg)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0aed3d8a24aa41f6b622a85a170b1823)](https://app.codacy.com/manual/SzigetiJ/biguint?utm_source=github.com&utm_medium=referral&utm_content=SzigetiJ/biguint&utm_campaign=Badge_Grade_Dashboard)
[![codecov](https://codecov.io/gh/SzigetiJ/biguint/branch/master/graph/badge.svg)](https://codecov.io/gh/SzigetiJ/biguint)

C library providing fixed length unsigned integer types longer than 64 bits.

## Features

Currently libbiguint provides the following types:
* `BigUInt128` (128 bits)
* `BigUInt256` (256 bits)
* `BigUInt384` (384 bits)
* `BigUInt512` (512 bits)

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

## Examples

Check out directory ![examples](examples).

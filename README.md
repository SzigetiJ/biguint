## (Not just) Big Unsigned Integers
_But also Big Signed Integers and Big Decimal numbers._

![GitHub](https://img.shields.io/github/license/SzigetiJ/biguint)
![C/C++ CI](https://github.com/SzigetiJ/biguint/workflows/C/C++%20CI/badge.svg)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0aed3d8a24aa41f6b622a85a170b1823)](https://app.codacy.com/gh/SzigetiJ/biguint/dashboard)
[![codecov](https://codecov.io/gh/SzigetiJ/biguint/branch/master/graph/badge.svg)](https://codecov.io/gh/SzigetiJ/biguint)
[![GitHub code size](https://img.shields.io/github/languages/code-size/SzigetiJ/biguint)](https://github.com/SzigetiJ/biguint)
![GitHub repo size](https://img.shields.io/github/repo-size/SzigetiJ/biguint)
![GitHub commit activity](https://img.shields.io/github/commit-activity/y/SzigetiJ/biguint)
![GitHub issues](https://img.shields.io/github/issues/SzigetiJ/biguint)
![GitHub closed issues](https://img.shields.io/github/issues-closed/SzigetiJ/biguint)

C library providing fixed length integer types longer than 32/64 bits.
The key design concepts of libbiguint are:

* Fast operations.
* Versatile function set.
* Supporting different hardware platforms.
* No dynamic memory handling.
* Only essential dependencies.
* Configurability and modularity.

All these make libbiguint suitable for embedded systems.

## Features

### BigUInt

libbiguint provides the following unsigned integer types:

* `BigUInt128` (128 bits)
* `BigUInt256` (256 bits)
* `BigUInt384` (384 bits)
* `BigUInt512` (512 bits)
* `BigUInt<number>` (1024 or even more bits)

All the provided types are accompanied by the following functions:

* addition and subtraction (add, sub, inc, dec, adc, sbc);
* multiplication and division (mul, dmul, div/mod);
* bit shift operations (shl, shr, rol, ror);
* bitwise operations (and, or, xor, not);
* bitwise manipulation (get, set, clr, overwrite);
* comparison (lt, eq, eqz);
* parsing and printing (from/to hex and dec strings);
* default and standard _constructors_ (initializer functions).

The source code of type `BigUInt128` is written in general manner.
The source of all other biguint types are generated codes derived from `BigUInt128`.
Types `BigUInt256`, `BigUInt384`, `BigUInt512` and `BigUInt<N>` and
adherent functions are optionally generated:
the 256, 384 and 512 bit wide types are enabled by default, the N bit wide type is disabled.
See `configure --help` for details.

### BigInt

There are no explicit `BigInt<number>` types.
We can store the signed big integers in BigUInt types.
And most of the BigUInt functions work perfectly with BigInts.
However, there are some functions which work differently for signed and unsigned integers,
therefore they have their BigInt variants:

* parsing and printing (only dec strings);
* comparison (lt, ltz);
* division (div) and inversion (negate).

### BigDecimal

Based on the corresponding BigUInt type, the following BigDecimal types are available:

* `BigDecimal128`
* `BigDecimal256`
* `BigDecimal384`
* `BigDecimal512`
* `BigDecimal<number>`

The functions accompanying these types are:

* addition and subtraction (add, sub);
* multiplication and division (mul, div, div_fast);
* precision adjustment;
* comparison (lt, eq);
* parsing and printing (only decimal format is supported).

All BigDecimal numbers are treated as signed values.

## Installation

### Get the source

Either clone the git source or download and extract the zip.
Cloning git source is preferred. It is easier to update.

### Autotools preparation

First, you need to run `aclocal`.

Next, run `autoconf`.

Finally, run `automake --add-missing`.

```sh
aclocal
autoconf
automake --add-missing
```

### Configure & Install

The [INSTALL](INSTALL) file already describes how to run the `configure` script.

Installation prefix, compiler, target platform, etc. can be overridden at this step.

```sh
./configure
make
make install
```

#### Different configurations simultaneously

The `configure` script supports handling different build profiles simultaneously
(see [VPATH Builds](https://www.gnu.org/software/automake/manual/html_node/VPATH-Builds.html)).
It generates the outputs (Makefiles) in the current working directory, whereever the `configure` script has been called.
Executing `make` with these generated Makefiles
will put the build output in the directory, where the Makefiles reside.
Well, except for `make install`, of course.

You can create and manage multiple profiles, e.g., a **Debug** and a **Release** profile,
within the base directory of the project:

```sh
mkdir -p dist/Debug
cd dist/Debug
../../configure CFLAGS="-O0 -g -W -Wall"
make
cd ../..
```

and

```sh
mkdir -p dist/Release
cd dist/Release
../../configure CFLAGS="-O2"
make
make install
cd ../..
```

Cross compilation is also supported.
All you have to do is to create a profile for the desired target.
Note, you have to give options `--host` and `--build` to `configure`, see the
[online manual](https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.70/html_node/Hosts-and-Cross_002dCompilation.html#Hosts-and-Cross_002dCompilation).

## How to use

Read some words about the naming conventions of functions [here](../../wiki/Naming).
### Use case #1: Summing very long values (C strings)

And getting the sum in C string (i.e., 0-terminated char array) as well.

```c
#include <string.h>
#include "biguint128.h"

#define BUFLEN 42
int main() {
 const char a_str[]="123456789012345678901234567890";
 const char b_str[]="111111111111111111111111111111";
 char res_str[BUFLEN];

 BigUInt128 a = biguint128_ctor_deccstream(a_str, strlen(a_str));
 BigUInt128 b = biguint128_ctor_deccstream(b_str, strlen(b_str));

 BigUInt128 res = biguint128_add(&a, &b);
 res_str[biguint128_print_dec(&res, res_str, BUFLEN)]=0;
 // now res_str contains the sum of a and b in decimal format.
}
```

### Use case #2: Multiplying beyond 64 bits

10^21 is greater than 2^64.
```c
#include <stdio.h>
#include "biguint128.h"

int main() {
 BigUInt128 a = biguint128_value_of_uint(10000000);

 BigUInt128 asquare = biguint128_mul(&a,&a);
 BigUInt128 acube = biguint128_mul(&a,&asquare);

 printf("Highest bit set in a: %d\n", (int)biguint128_msb(&a));
 printf("Highest bit set in a^2: %d\n", (int)biguint128_msb(&asquare));
 printf("Highest bit set in a^3: %d\n", (int)biguint128_msb(&acube));

 return 0;
}
```

## Examples

Check out the ![examples](examples) directory.

## Big Unsigned Integers

![GitHub](https://img.shields.io/github/license/SzigetiJ/biguint)
![C/C++ CI](https://github.com/SzigetiJ/biguint/workflows/C/C++%20CI/badge.svg)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0aed3d8a24aa41f6b622a85a170b1823)](https://app.codacy.com/gh/SzigetiJ/biguint/dashboard)
[![codecov](https://codecov.io/gh/SzigetiJ/biguint/branch/master/graph/badge.svg)](https://codecov.io/gh/SzigetiJ/biguint)

C library providing fixed length unsigned integer types longer than 64 bits.

## Features

Currently libbiguint provides the following types:
* `BigUInt128` (128 bits)
* `BigUInt256` (256 bits)
* `BigUInt384` (384 bits)
* `BigUInt512` (512 bits)
* `BigUInt<number>` (1024 or even more bits)

All the provided types are accompanied by the following functions:
* addition and subtraction (add, sub, inc, dec);
* multiplication and division (mul, div/mod);
* bit shift operations (shl, shr);
* bitwise operations (and, or, xor, not);
* bitwise manipulation (get, set, clr, overwrite);
* comparison (lt, eq);
* parsing and printing (from/to hex and dec strings);
* default and standard _constructors_ (initializer functions).

The source code of type `BigUInt128` is written in general manner.
The source of all other biguint types are generated codes derived from `BigUInt128`.

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

#### Different configurations at the same

The `configure` script supports different build profiles:
it generates the outputs (Makefiles) in the current working directory;
and executing `make` with the generated Makefiles
will put their output in the container directory of the given Makefiles.
Well, except for `make install`, of course.

You can create, e.g., a **Debug** and a **Release** profile within the base directory of the project:

```sh
mkdir Debug
cd Debug
../configure CFLAGS="-O0 -g -W -Wall"
make
cd ..
```

and

```sh
mkdir Release
cd Release
../configure CFLAGS="-O2"
make
make install
cd ..
```

Cross compilation is also supported. You can create a profile for the target.
Note, you have to give options `--host` and `--build` to `configure` see the
[online manual](https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.70/html_node/Hosts-and-Cross_002dCompilation.html#Hosts-and-Cross_002dCompilation).

## How to use

Use case: Summing very long values (C strings).
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

## Examples

Check out the ![examples](examples) directory.

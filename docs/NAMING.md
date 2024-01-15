## Naming convention in libbiguint

### Types

The base storage type that is used within the BigXXX types is `UInt`,
which is a type alias for an unsigned integer type.
What C type is this `UInt`, depends on the configuation parameters.
It can be either `unsigned int`, `uint32_t` or `uint64_t`.
See [uint_types.h](../src/uint_types.h).

Also, we use type alias `buint_bool` for `bool` and `buint_size_t` for `size_t`
all over libbiguint.

There are two type groups,
`BigUIntNNN` for big integers and
`BigDecimalNNN` for big numbers with decimal digits.
Here, `NNN` denotes the capacity of the type (in bits).
`BigUIntNNN` is merely a static array of `UInt`, whereas
`BigDecimalNNN` is a compound of a `BigUIntNNN` and a number storing the _precision_ value.
Whenever we refer here to both `BigUIntNNN` and `BigDecimalNNN` types, we use the term _BigXXX_.

Some functions return a pair of values (e.g., integer division returns quotient and remainder).
For that purpose we use types `BigUIntPairNNN`, which is a compound of two `BigUIntNNN` members.
Sometimes we know in advance that the second member of the pair will fit into a base type
(e.g., in case of predefined division with a given divisor),
hence we also defined the types `BigUIntTinyPairNNN` (see the _typedef_ section in [biguint128.h](../src/biguint128.h)).

As already noted in [README](../README.md),
in the bare source you will find only `NNN=128`, i.e.,
`BigUInt128` / `BigDecimal128` types.
All the other types and functions with `NNN=256`, `NNN=512`, etc. are generated.

### Functions

The function names always have the following structure: `<TYPE_GROUP><NNN>_<FUNC>[_<SUFFIX>]`.
Here

 * `<TYPE_GROUP>` denotes the _BigXXX_ type group
 and is either `biguint` for unsigned, `bigint` for signed interers; or `bigdecimal`.

 * `<NNN>` refers to the capacity number of the storage type, so it is `128`, etc.

 * `<FUNC>` is the (commonly used) name of the function or the abbreviation of it
 (like `ctor` for _constructor_ or `dmul` for _multiplication with double precision result_).

 * `_<SUFFIX>` denotes a function variant. This is what we discuss later in this page.

#### Function parameter types

As a rule of thumb, whenever possible,
_BigXXX_ parameters are passed as constant references (e.g., `const BigUInt128 *a`) to functions
There are two exceptions:

 1. The function is an _assignment_ or _replacement_ variant.
 In this case the parameter is passed as reference (e.g., `BigDecimal128 *a`).

 1. The function is _pass-by-value_ variant.
 In this case the parameter is passed as value (e.g., `const BigUInt128 a`).

#### Function variants

Functions variants are denoted by suffix in the function name.
There are some general suffices which are used frequently.

 * `_assign` denotes that the function is an assignment:
 the prototype of the original function is modified at two points:
   * the first parameter is passed by R/W reference;
   * the return type is e reference (to the first parameter).

 * `_tiny` is a subset of assignment.
 Sometimes, if the value range of the second parameter of the function is constrained,
 we can achieve faster operation. For example, adding an `UInt` to `BigUIntNNN` or
 shifting `BigUIntNNN` with only some bits.

 * `_replace` variants get an additional parameter, of type _(write only) reference_,
 to the head of the parameter list of the original function.
 They (should) return this reference.
 So based on `BigUInt128 biguint128_add(const BigUInt128 *a, const BigUInt128 *b);`
 the function `BigUInt128 *biguint128_add_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b)`
 is created.

 * `v` denotes functions with BigXXX parameters passed by value.

Note, they look like, but are not variants as described above:

 * `ctor_...` are constructor variants.
 There is a special case, `bigdecimal128_ctor_precv`,
 when function variant is applied to constructor variant.

 * `print_...` functions, here the `<FUNC>` is `print_dec` or `print_hex`, and they have `v` suffixed variant.

Again, there are some exceptions with the variants,
where the variant is not shown in the function name suffix:

 * `biguintNNN_inc` and `biguintNNN_dec` are of variant `_tiny`.
 On the one hand, implementing the original base function or other variants would not have much sense,
 as they would not give as much performance enhancement compared to
 `biguintNNN_add` / `biguintNNN_sub`.
 On the other hand, adding the `_tiny` suffix to these function names,
 would have resulted in function names that are not proportional to the complexity of the operation.

 * `biguintNNN_sbit`, `biguintNNN_cbit` and `biguintNNN_obit` are of variant `_assign`.
 Historically, they were named and prototyped that way.
 Honestly, `sbit` / `cbit` meaning _set / clear bit_ suggest a modification on a given value,
 and not creating a new value.

 * `biguintNNN_import` is of variant `_replace`.
 That is because `biguintNNN_export` is the pair of this function,
 and we decided that showing the relationship in the naming is more important
 than conforming to naming conventions.

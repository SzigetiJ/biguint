#include "biguint128.h"
#include "intio.h"
#include "uint.h"
#include <stdio.h>

#define FOREACHCELL(i) for (buint_size_t i=0u; i<BIGUINT128_CELLS; ++i)
#define UINT_BYTES sizeof(UInt)
#define UINT_BITS (8 * UINT_BYTES)

BigUInt128 biguint128_ctor_default() {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i]=0;
 }
 return retv;
};

BigUInt128 biguint128_ctor_unit() {
 BigUInt128 retv = biguint128_ctor_default();
 retv.dat[0]=1;
 return retv;

}

BigUInt128 biguint128_ctor_standard(const UInt *a) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i]=a[i];
 }
 return retv;
};

BigUInt128 biguint128_ctor_copy(const BigUInt128 *a) {
 return biguint128_ctor_standard(a->dat);
}

BigUInt128 biguint128_ctor_hexcstream(const char *a, buint_size_t len) {
 BigUInt128 retv=biguint128_ctor_default();
 buint_bool ready = 0;
 for (buint_size_t i = 0; !ready && i < BIGUINT128_CELLS; ++i) {
  for (buint_size_t j = 0; !ready && j < UINT_BYTES; ++j) {
   buint_size_t offset = 2 * (UINT_BYTES * i + j);
   if (offset < len) {
    unsigned char x;
    if (offset + 1 < len) {
     get_hexbyte(a + len - offset - 2, &x);
    } else {
     get_digit(a[len - offset - 1], 16, &x);
    }
    retv.dat[i]|= x << 8*j;
   } else {
    ready = 1;
   }
  }
 }
 return retv;
}

BigUInt128 biguint128_ctor_deccstream(const char *a, buint_size_t len) {
 BigUInt128 retv=biguint128_ctor_default();
 BigUInt128 base = biguint128_value_of_uint(10);
 for (buint_size_t i = 0; i < len; ++i) {
  unsigned char d;
  get_digit(a[i], 10, &d);
  BigUInt128 dx = biguint128_value_of_uint(d);
  retv = biguint128_mul(&retv, &base);
  retv = biguint128_add(&retv, &dx);
 }
 return retv;
}


BigUInt128 biguint128_value_of_uint(UInt a) {
 BigUInt128 retv = biguint128_ctor_default();
 retv.dat[0] = a;
 return retv;
}

BigUInt128 biguint128_add(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 buint_bool carry = 0;
 FOREACHCELL(i) {
  retv.dat[i] = uint_add(a->dat[i], b->dat[i], &carry);
 }
 return retv;
};

BigUInt128 biguint128_sub(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 buint_bool carry = 0;
 FOREACHCELL(i) {
  retv.dat[i] = uint_sub(a->dat[i], b->dat[i], &carry);
 }
 return retv;
};

BigUInt128 biguint128_shl(const BigUInt128 *a, const buint_size_t shift) {
 BigUInt128 retv = biguint128_ctor_default();
 buint_size_t bit_shift = shift % UINT_BITS;
 buint_size_t byte_shift = shift / UINT_BITS;

 FOREACHCELL(i) {
  UIntPair x = uint_split(a->dat[i], UINT_BITS - bit_shift);
  if (i < BIGUINT128_CELLS - byte_shift) {
   retv.dat[i + byte_shift]|= x.second << bit_shift;
   if (i < BIGUINT128_CELLS - byte_shift - 1) {
    retv.dat[i + byte_shift + 1]|= x.first >> UINT_BITS - bit_shift;
   }
  }
 }
 return retv;
};

BigUInt128 biguint128_shr(const BigUInt128 *a, const buint_size_t shift) {
 BigUInt128 retv = biguint128_ctor_default();
 buint_size_t bit_shift = shift % UINT_BITS;
 buint_size_t byte_shift = shift / UINT_BITS;

 FOREACHCELL(i) {
  UIntPair x = uint_split(a->dat[i], bit_shift);
  if (byte_shift <= i) {
   retv.dat[i - byte_shift]|= x.first >> bit_shift;
   if (byte_shift + 1 <= i) {
    retv.dat[i - byte_shift - 1]|= x.second << UINT_BITS - bit_shift;
   }
  }
 }
 return retv;
};

BigUInt128 biguint128_and(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] & b->dat[i];
 }
 return retv;
};

BigUInt128 biguint128_or(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] | b->dat[i];
 }
 return retv;
};

BigUInt128 biguint128_not(const BigUInt128 *a) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = ~a->dat[i];
 }
 return retv;
};

BigUInt128 biguint128_xor(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] ^ b->dat[i];
 }
 return retv;
};

BigUInt128 biguint128_mul(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv = biguint128_ctor_default();
 BigUInt128 carry = biguint128_ctor_default();
 FOREACHCELL(i) {
  FOREACHCELL(j) {
   buint_size_t k = i + j;
   if (k < BIGUINT128_CELLS) {
    UIntPair kmul = uint_mul(a->dat[i], b->dat[j]);
    buint_bool cx0 = 0;
    buint_bool cx1 = 0;
    retv.dat[k] = uint_add(retv.dat[k], kmul.second, &cx0);
    if (k + 1 < BIGUINT128_CELLS) {
     if (cx0) {
      ++carry.dat[k + 1];
     }
     retv.dat[k + 1] = uint_add(retv.dat[k + 1], kmul.first, &cx1);
     if (cx1 && k + 2 < BIGUINT128_CELLS) {
      ++carry.dat[k + 2];
     }
    }
   }
  }
 }
 BigUInt128 rx = biguint128_add(&retv, &carry);
 return rx;
};

BigUIntPair128 biguint128_div(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 div = biguint128_ctor_default();
 BigUIntPair128 retv;
 retv.first = div;
 buint_size_t msb_a = biguint128_msb(a);
 buint_size_t msb_b = biguint128_msb(b);
 if (msb_a < msb_b) {
  retv.second = biguint128_ctor_copy(a);
 } else {
  buint_size_t xbit = msb_a - msb_b;
  BigUInt128 xobj = biguint128_ctor_copy(a);
  BigUInt128 xdiv = biguint128_shl(b, xbit);
  for (buint_size_t i=0; i <= xbit; ++i) {
   if (!biguint128_lt(&xobj, &xdiv)) {
    biguint128_sbit(&retv.first, xbit - i);
    xobj = biguint128_sub(&xobj, &xdiv);
   }
   xdiv = biguint128_shr(&xdiv, 1);
  }
  retv.second = xobj;
 }
 return retv;
}

buint_bool biguint128_lt(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  buint_size_t j = BIGUINT128_CELLS - i - 1;
  if (a->dat[j]<b->dat[j]) return 1;
  if (b->dat[j]<a->dat[j]) return 0;
 }
 return 0;
};

buint_bool biguint128_eq(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  if (a->dat[i]!=b->dat[i]) return 0;
 }
 return 1;
}

buint_size_t biguint128_msb(const BigUInt128 *a) {
 buint_size_t j;
 buint_bool found = 0;
 FOREACHCELL(i) {
  j = BIGUINT128_CELLS - i - 1;
  if (0 < a->dat[j]) {
   found = 1;
   break;
  }
 }
 if (!found) {
  return 0;
 }
 return j * 8 * sizeof(UInt) + uint_msb(a->dat[j]);
};

void biguint128_sbit(BigUInt128 *a, buint_size_t bit) {
 buint_size_t dat_byte = bit / UINT_BITS;
 buint_size_t dat_bit = bit % UINT_BITS;
 a->dat[dat_byte]|= 1<<dat_bit;
}

buint_size_t biguint128_print_hex(BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t retv = biguint128_msb(a)/4 + 1;
 if (buf_len < retv) {
  return -1;
 }
 buint_bool ready = 0;
 for (buint_size_t i=0; !ready && i<BIGUINT128_CELLS; ++i) {
  for (buint_size_t j=0; !ready && j<UINT_BYTES; ++j) {
   buint_size_t offset = 2 * (i * UINT_BYTES + j);
   if (retv <= offset) {
    ready = 1;
    break;
   }
   if (offset + 1 < retv) {
    set_hexbyte(buf + retv - offset - 2, a->dat[i]>>8*j & 0xFF);
   } else {
    set_hexdigit(buf + retv - offset - 1, a->dat[i]>>8*j & 0xF);
   }
  }
 }
 return retv;
}

buint_size_t biguint128_print_dec_anywhere(BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset) {
 buint_bool ready = 0;
 BigUInt128 divisor = biguint128_value_of_uint(10);
 BigUInt128 zero = biguint128_value_of_uint(0);
 BigUInt128 temp = biguint128_ctor_copy(a);
 for (buint_size_t i=0; !ready && i < buf_len; ++i) {
  buint_size_t buf_idx = buf_len - i - 1;
  BigUIntPair128 res = biguint128_div(&temp, &divisor);
  unsigned char d = res.second.dat[0];
  set_decdigit(buf + buf_idx, d);
  ready = biguint128_eq(&zero, &res.first);
  temp = res.first;
  *offset = buf_idx;
 }
 return buf_len - *offset;
}
buint_size_t biguint128_print_dec(BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t offset;
 buint_size_t size = biguint128_print_dec_anywhere(a, buf, buf_len, &offset);
 for (buint_size_t i = 0; i < size; ++i) {
  buf[i] = buf[offset + i];
 }
 return size;
}

#undef FOREACHCELL
#undef UINT_BYTES
#undef UINT_BITS


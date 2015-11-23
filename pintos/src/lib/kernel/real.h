#ifndef __LIB_KERNEL_REAL_H
#define __LIB_KERNEL_REAL_H

// will be implemented by taeguk.

typedef int32_t real;

real real_from_int (int32_t n);
int32_t real_to_int (real x);

real real_add_rr (real x, real y);
real real_add_ri (real x, int32_t n);

real real_sub_rr (real x, real y);    // subtract y from x
real real_sub_ri (real x, int32_t n);

real real_mul_rr (real x, real y);
real real_mul_ri (real x, int32_t n);

real real_div_rr (real x, real y);    // divide x by y
real real_div_ri (real x, int32_t n);

#endif

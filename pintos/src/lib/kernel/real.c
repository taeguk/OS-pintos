// will be implemented by taeguk.

#include "real.h"

#define FP_SIGN_BITS  1
#define FP_P_BITS     17
#define FP_Q_BITS     14

#define FP_F    (1 << FP_Q_BITS)

real real_from_int (int32_t n)
{
  return n * FP_F;
}

int32_t real_to_int (real x)
{
  return x / FP_F;
}

int32_t real_to_int_nearest (real x)
{
  return x >= 0 ?
    (x + FP_F / 2) / FP_F :
    (x - FP_F / 2) / FP_F ;
}

real real_add_rr (real x, real y)
{
  return x + y;
}

real real_add_ri (real x, int32_t n)
{
  return real_add_rr (x, real_from_int (n));
}

real real_sub_rr (real x, real y)
{
  return x - y;
}

real real_sub_ri (real x, int32_t n)
{
  return real_sub_rr (x, real_from_int (n));
}

real real_mul_rr (real x, real y)
{
  return (real) (((int64_t) x) * y / FP_F);
}

real real_mul_ri (real x, int32_t n)
{
  return x * n;
}

real real_div_rr (real x, real y)
{
  return (real) (((int64_t) x) * FP_F / y);
}

real real_div_ri (real x, int32_t n)
{
  return x / n;
}


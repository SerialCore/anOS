#include <lib/math/basicmath.h>

int math_div(int dividend, int divisor, unsigned int* remainder)
{
    int quotient = 1;
    int origdiv = divisor;

    if (dividend == divisor)
    {
        *remainder = 0;
        return 1;
    }
    else if (dividend < divisor)
    {
        *remainder = dividend;
        return 0;
    }

    while (divisor <= dividend)
    {
        divisor <<= 1;
        quotient <<= 1;
    }

    if (dividend < divisor)
    {
        divisor >>= 1;
        quotient >>= 1;
    }

    quotient = quotient + math_div(dividend - divisor, origdiv, remainder);

    return quotient;
}
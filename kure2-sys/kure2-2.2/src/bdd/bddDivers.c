#include "KureImpl.h"
#include <math.h>

/*!
 * Returns the number of bits necessary to store the number in binary.
 */
KURE_INLINE int number_of_vars2 (unsigned long n)
{
	return (int)ceil(log2(n));
}

/*!
 * Return TRUE if the number is a power of 2 and FALSE otherwise.
 */
KURE_INLINE Kure_bool int_is_pow2 (unsigned long n)
{
	double l = log2(n);
	return (double)(int)l == l;
}



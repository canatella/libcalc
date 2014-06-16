#ifndef _LIBCALC_PRIV_H_
#define _LIBCALC_PRIV_H_

#include <stdio.h>

#include "libcalc.h"

#define tr(format, ...)  fprintf(stderr, format " (%s:%u)\n", ## __VA_ARGS__, __FILE__, __LINE__)

/**
 * Check that the library context is in a valid state.
 */
#define assert_calc(C) do {                     \
    assert(C);                                  \
    assert(C->size);                            \
    assert(C->top <= C->size);                  \
    } while (0)

#endif /* _LIBCALC_PRIV_H_ */

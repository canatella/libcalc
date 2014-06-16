#ifndef _LIBCALC_PRIV_H_
#define _LIBCALC_PRIV_H_

#include <stdio.h>
#include <limits.h>

#include "libcalc.h"

#define tr(format, ...)  fprintf(stderr, format " (%s:%u)\n", ## __VA_ARGS__, __FILE__, __LINE__)

/**
 * Check that the library context is in a valid state.
 */
#define assert_calc(C) do {                     \
    assert(C);                                  \
    assert(C->stack);                           \
    assert(C->size);                            \
    assert(C->top <= C->size);                  \
    } while (0)

/**
 * Number of available operations
 */
#define CA_OPERATION_COUNT (CA_OP_RIGHT_SHIFT + 1)

/**
 * Check that an operation is valid.
 */
#define assert_ca_operation(O) assert(CA_OPERATION_COUNT > (O))

/**
 * The maximum available value
 */
#define CA_VALUE_MAX LONG_MAX

/**
 * The minimum available value
 */
#define CA_VALUE_MIN LONG_MIN

#endif /* _LIBCALC_PRIV_H_ */

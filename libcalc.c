#include <assert.h>
#include <stdlib.h>

#include "libcalc_priv.h"

int ca_initialize(ca_calc_t *calc, size_t size)
{
    assert(calc);
    assert(size);
    calc->stack = calloc(size, sizeof(ca_value_t));
    if (calc->stack == NULL) {
        tr("unable to create stack: %m");
        return -1;
    }
    calc->size = size;
    calc->top = 0;
    return 0;
}

void ca_cleanup(ca_calc_t *calc)
{
    assert(calc);
    free(calc->stack);
}

size_t ca_space_left(ca_calc_t *calc)
{
    assert_calc(calc);
    return calc->size - calc->top;
}

void ca_push(ca_calc_t *calc, ca_value_t value)
{
    assert_calc(calc);
    assert(calc->size < calc->top);
    calc->stack[calc->top] = value;
    calc->top += 1;
}

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

ca_value_t ca_top(ca_calc_t *calc)
{
    assert_calc(calc);
    assert(calc->top > 0);
    return calc->stack[calc->top - 1];
}

void ca_push(ca_calc_t *calc, ca_value_t value)
{
    assert_calc(calc);
    /* ensure there is space left */
    assert(calc->top < calc->size);
    calc->stack[calc->top] = value;
    calc->top += 1;
}

unsigned ca_remove(ca_calc_t *calc, unsigned count)
{
    assert_calc(calc);

    if (count == 0 || count > calc->top)
        count = calc->top;

    calc->top -= count;
    return count;
}

ca_value_t ca_pop(ca_calc_t *calc)
{
    assert_calc(calc);
    assert(calc->top);
    calc->top -= 1;
    return calc->stack[calc->top];
}

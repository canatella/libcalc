#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

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

/**
 * Ensure that there are at least count values on the stack
 */
static int ca_check_values(ca_calc_t *calc, unsigned count)
{
    assert(calc);
    if (calc->top < count) {
        tr("stack should hold at least %u operand", count);
        return -1;
    }
    return 0;
}

/**
 * Return the first operand
 */
#define ca_first(C) (C->stack[C->top - 2])
/**
 * Return the second operand
 */
#define ca_second(C) ca_top(C)

/**
 * Add the two top values.
 */
static int ca_op_add(ca_calc_t *calc)
{
    if (ca_check_values(calc, 2))
        return -1;

    ca_value_t x = ca_first(calc);
    ca_value_t y = ca_second(calc);

    if ((y > 0 && x > CA_VALUE_MAX - y) || (y < 0 && x < CA_VALUE_MIN - y)) {
        tr("addition would overflow");
        return -1;
    }

    ca_value_t result = x + y;
    ca_remove(calc, 2);
    ca_push(calc, result);
    return 0;
}

/**
 * Substract the two top values.
 */
static int ca_op_substract(ca_calc_t *calc)
{
    if (ca_check_values(calc, 2))
        return -1;

    ca_value_t x = ca_first(calc);
    ca_value_t y = ca_second(calc);

    if ((y > 0 && x < CA_VALUE_MIN + y) || (y < 0 && x > CA_VALUE_MAX + y)) {
        tr("substraction would overflow");
        return -1;
    }

    ca_value_t result = x - y;
    ca_remove(calc, 2);
    ca_push(calc, result);
    return 0;
}

/**
 * Multiply the two top values.
 */
static int ca_op_multiply(ca_calc_t *calc)
{
    if (ca_check_values(calc, 2))
        return -1;

    ca_value_t x = ca_first(calc);
    ca_value_t y = ca_second(calc);

    if (y != 0 && ((((x > 0 && y > 0) || (x < 0 || y < 0)) && (y > 0 ? x > CA_VALUE_MAX / y : x < CA_VALUE_MAX / y)) ||
                   (((x > 0 && y < 0) || (x < 0 || y > 0)) && (y > 0 ? x < CA_VALUE_MIN / y : x > CA_VALUE_MIN / y)))) {
        tr("multiplication would overflow");
        return -1;
    }

    ca_value_t result = x * y;
    ca_remove(calc, 2);
    ca_push(calc, result);
    return 0;
}

/**
 * Divide the two top values.
 */
static int ca_op_divide(ca_calc_t *calc)
{
    if (ca_check_values(calc, 2))
        return -1;

    ca_value_t x = ca_first(calc);
    ca_value_t y = ca_second(calc);

    if (y == 0) {
        tr("cannot divide by 0");
        return -1;
    }

    ca_value_t result = x / y;
    ca_remove(calc, 2);
    ca_push(calc, result);
    return 0;
}

/**
 * Calculate the square root of the top value.
 */
static int ca_op_square_root(ca_calc_t *calc)
{
    if (ca_check_values(calc, 1))
        return -1;

    ca_value_t x = ca_pop(calc);

    if (x < 0) {
        tr("complex numbers are not supported, cannot fetch square root of negative numbers");
        return -1;
    }

    /* lets keep it simple, do a binary search */
    ca_value_t min = 0, max = x;
    ca_value_t middle = ((min + max) >> 1) + 1;

    do {
        ca_value_t d = x / middle;
        if (middle == d)
            break;
        else if (middle > d)
            max = middle;
        else
            min = middle;
        middle = (min + max) >>  1;
    } while (max - min > 1);

    ca_push(calc, middle);
    return 0;
}

/**
 * Calculate the modulo of the two top values.
 */
static int ca_op_modulo(ca_calc_t *calc)
{
    if (ca_check_values(calc, 2))
        return -1;

    ca_value_t x = ca_first(calc);
    ca_value_t y = ca_second(calc);

    if (y == 0) {
        tr("cannot calculate modulo by 0");
        return -1;
    }

    ca_value_t result = x % y;
    ca_remove(calc, 2);
    ca_push(calc, result);
    return 0;
}

static int (*operations[])(ca_calc_t *calc) = {
    ca_op_add,
    ca_op_substract,
    ca_op_multiply,
    ca_op_divide,
    ca_op_square_root,
    ca_op_modulo
};

int ca_operate(ca_calc_t *calc, ca_operation_t op)
{
    assert(calc);
    assert(operations[op]);
    return operations[op](calc);
}

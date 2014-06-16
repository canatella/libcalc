#ifndef _LIBCALC_H_
#define _LIBCALC_H_

#include <stddef.h>

/**
 * The values to operate on.
 */
typedef long int ca_value_t;

/**
 * The maximum available value
 */
#define CA_VALUE_MAX LONG_MAX

/**
 * The minimum available value
 */
#define CA_VALUE_MIN LONG_MIN

/**
 * The available operations.
 */
typedef enum ca_operation {
    CA_OP_ADD,
    CA_OP_SUBSTRACT,
    CA_OP_MULTIPLY,
    CA_OP_DIVIDE,
    CA_OP_SQUARE_ROOT,
    CA_OP_MODULO,
    CA_OP_LEFT_SHIFT,
    CA_OP_RIGHT_SHIFT
} ca_operation_t;

/**
 * The library context.
 */
typedef struct ca_calc {
    /** The stack */
    ca_value_t *stack;
    /** Size of the stack */
    size_t size;
    /** Index of the top of the stack */
    size_t top;
} ca_calc_t;

/**
 * Initialize the library context.
 *
 * @param size size of the stack, must be greater than 0.
 * @return 0 on success, -1 otherwise.
 */
int ca_initialize(ca_calc_t *calc, size_t size) __attribute__ ((nonnull(1)));

/**
 * Cleanup the library context.
 */
void ca_cleanup(ca_calc_t *calc) __attribute__ ((nonnull(1)));

/**
 * Return the number of element on the stack
 */
 __attribute__ ((nonnull(1)))
static inline size_t ca_count(ca_calc_t *calc)
{
    return calc->top;
}

/**
 * Return the space left of the stack
 *
 * @return the number of ca_value_t that can be pushed on the stack.
 */
size_t ca_space_left(ca_calc_t *calc) __attribute__ ((nonnull(1)));

/**
 * Push a value on the stack.
 *
 * The user should make sure that there is enough room on the stack
 * before calling this function.
 *
 * @param calc the library context
 * @param value the value to push on the stack
 */
void ca_push(ca_calc_t *calc, ca_value_t value) __attribute__ ((nonnull(1)));

/**
 * Pop a value from the stack.
 *
 * The user should make sure that there are enough element on the
 * stack before calling this function.
 *
 * @param calc the library context
 * @return the value popped from the stack
 */
ca_value_t ca_pop(ca_calc_t *calc) __attribute__ ((nonnull(1)));

/**
 * Pop values from the stack
 *
 * @param count the number of values to pop, 0 for emptying the stack*
 * @return the
 */
unsigned ca_remove(ca_calc_t *calc, unsigned count) __attribute__ ((nonnull(1)));

/**
 * Look at the top of the stack
 */
ca_value_t ca_top(ca_calc_t *calc) __attribute__ ((nonnull(1)));

/**
 * Apply an operation to elements on the stack
 */
int ca_operate(ca_calc_t *calc, ca_operation_t op) __attribute__ ((nonnull(1)));


#endif /* _LIBCALC_H_ */

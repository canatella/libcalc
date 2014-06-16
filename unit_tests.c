#include "libcalc_priv.h"
#include "testsuite.h"

#include <stdbool.h>
#include <errno.h>
#include "libcalc.c"

/**
 * Set to true so that a mocked function succeeds.
 */
static bool succeed;

/**
 * Incremented each time calloc is called.
 */
static unsigned calloc_count;

/**
 * Decremented each time free is called.
 */
static unsigned free_count;

/**
 * A context on which to do the tests
 */
ca_value_t stack[100] = { 0, };
ca_calc_t calc = {
    .stack = stack
};

/**
 * Mock calloc calls so we can make it fail and count them.
 */
void *__real_calloc(size_t nmemb, size_t size);
void *__wrap_calloc(size_t nmemb, size_t size)
{
    void *retval = NULL;
    if (succeed) {
        retval = __real_calloc(nmemb, size);
        check(retval, "calloc should allocate memory");
    } else {
        errno = ENOMEM;
    }
    calloc_count += 1;
    return retval;
}

/**
 * Mock free calls so we can count them.
 */
void __real_free(void *ptr);
void __wrap_free(void *ptr)
{
    __real_free(ptr);
    free_count += 1;
}

static void test_initialize_cleanup(void)
{
    ca_calc_t calc;

    /* check when calloc fails */
    succeed = false;
    check_failure(ca_initialize(&calc, 10));
    check(calloc_count == 1, "ca_initialize should call calloc");

    /* check when calloc succeeds */
    calloc_count = 0;
    succeed = true;
    check_success(ca_initialize(&calc, 10));
    check(calloc_count == 1, "ca_initialize should call calloc");
    check(calc.stack, "ca_initialize should allocate stack.");
    check(calc.size == 10, "ca_initialize should set stack size.");
    check(calc.top == 0, "ca_initialize should set stack top.");

    ca_cleanup(&calc);
    check(calloc_count == free_count, "ca_cleanup should free allocated memory");
}

static void test_space_left(void)
{
    calc.size = 10;
    calc.top = 3;
    check(ca_space_left(&calc) == 7, "ca_space_left should compute free space");
}

static void test_top(void)
{
    calc.size = 10;
    calc.top = 0;

    for (size_t i = 0; i < 10; i++) {
        stack[i] = i + 1;
        calc.top = stack[i];
        check(ca_top(&calc) == stack[i], "ca_top should return the value at the top of the stack");
    }
}

static void test_push_pop(void)
{
    calc.size = 10;
    calc.top = 0;

    ca_push(&calc, 1);
    check(calc.top == 1, "ca_push should increase top");
    check(stack[0] == 1, "ca_push should push value on the stack");

    ca_push(&calc, 5);
    check(calc.top == 2, "ca_push should increase top");
    check(stack[0] == 1, "ca_push should keep old values on the stack");
    check(stack[1] == 5, "ca_push should push value on the stack");

    check(ca_pop(&calc) == 5, "ca_pop should pop top value from the stack");
    check(calc.top == 1, "ca_pop should lower top");
    check(stack[0] == 1, "ca_pop should keep old value on the stack");

    check(ca_pop(&calc) == 1, "ca_pop should pop top value from the stack");
    check(calc.top == 0, "ca_pop should lower top");

}

static void test_remove(void)
{
    calc.size = 100;
    calc.top = 50;

    check(ca_remove(&calc, 13) == 13, "ca_remove should return the number of popped values");
    check(calc.top == 37, "ca_remove should lower top");

    check(ca_remove(&calc, 0) == 37, "ca_remove should return the number of popped values");
    check(calc.top == 0, "ca_remove should pop all values when using 0 as count");
}

static void test_check_values(void)
{
    calc.top = 3;
    check_failure(ca_check_values(&calc, 4));
    check_success(ca_check_values(&calc, 3));
    check_success(ca_check_values(&calc, 2));
    check_success(ca_check_values(&calc, 1));
    check_success(ca_check_values(&calc, 0));
}

static void test_op_add(void)
{
    calc.top = 0;
    check_failure(ca_op_add(&calc));
    calc.top = 1;
    check_failure(ca_op_add(&calc));

    calc.stack[0] = 30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_add(&calc));
    check(calc.top == 1, "add should remove two values and add the result on the stack");
    check(calc.stack[0] == 53, "add should put the addition result on the stack");

    calc.stack[0] = 30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_add(&calc));
    check(calc.top == 1, "add should remove two values and add the result on the stack");
    check(calc.stack[0] == 7, "add should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_add(&calc));
    check(calc.top == 1, "add should remove two values and add the result on the stack");
    check(calc.stack[0] == -7, "add should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_add(&calc));
    check(calc.top == 1, "add should remove two values and add the result on the stack");
    check(calc.stack[0] == -53, "add should put the addition result on the stack");

    /* check for overflows */
    calc.stack[0] = CA_VALUE_MAX;
    calc.stack[1] = 1;
    calc.top = 2;
    check_failure(ca_op_add(&calc));

    calc.stack[0] = CA_VALUE_MIN;
    calc.stack[1] = -1;
    calc.top = 2;
    check_failure(ca_op_add(&calc));

    calc.stack[0] = 1;
    calc.stack[1] = CA_VALUE_MAX;
    calc.top = 2;
    check_failure(ca_op_add(&calc));

    calc.stack[0] = -1;
    calc.stack[1] = CA_VALUE_MIN;
    calc.top = 2;
    check_failure(ca_op_add(&calc));
}

static void test_op_substract(void)
{
    calc.top = 0;
    check_failure(ca_op_substract(&calc));
    calc.top = 1;
    check_failure(ca_op_substract(&calc));

    calc.stack[0] = 30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_substract(&calc));
    check(calc.top == 1, "substract should remove two values and add the result on the stack");
    check(calc.stack[0] == 7, "substract should put the addition result on the stack");

    calc.stack[0] = 30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_substract(&calc));
    check(calc.stack[0] == 53, "substract should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_substract(&calc));
    check(calc.stack[0] == -53, "substract should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_substract(&calc));
    check(calc.stack[0] == -7, "substract should put the addition result on the stack");

    /* check for overflows */
    calc.stack[0] = CA_VALUE_MAX;
    calc.stack[1] = -1;
    calc.top = 2;
    check_failure(ca_op_substract(&calc));

    calc.stack[0] = CA_VALUE_MIN;
    calc.stack[1] = 1;
    calc.top = 2;
    check_failure(ca_op_substract(&calc));

    calc.stack[0] = -2;
    calc.stack[1] = CA_VALUE_MAX;
    calc.top = 2;
    check_failure(ca_op_substract(&calc));

    calc.stack[0] = 1;
    calc.stack[1] = CA_VALUE_MIN;
    calc.top = 2;
    check_failure(ca_op_substract(&calc));
}

static void test_op_multiply(void)
{
    calc.top = 0;
    check_failure(ca_op_multiply(&calc));
    calc.top = 1;
    check_failure(ca_op_multiply(&calc));

    calc.stack[0] = 30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_multiply(&calc));
    check(calc.top == 1, "multiply should remove two values and add the result on the stack");
    check(calc.stack[0] == 690, "multiply should put the addition result on the stack");

    calc.stack[0] = 30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_multiply(&calc));
    check(calc.stack[0] == -690, "multiply should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = 23;
    calc.top = 2;
    check_success(ca_op_multiply(&calc));
    check(calc.stack[0] == -690, "multiply should put the addition result on the stack");

    calc.stack[0] = -30;
    calc.stack[1] = -23;
    calc.top = 2;
    check_success(ca_op_multiply(&calc));
    check(calc.stack[0] == 690, "multiply should put the addition result on the stack");

    /* check for overflows */
    calc.stack[0] = CA_VALUE_MAX / 2 + 1;
    calc.stack[1] = 2;
    calc.top = 2;
    check_failure(ca_op_multiply(&calc));

    calc.stack[0] = CA_VALUE_MIN / 2 - 1;
    calc.stack[1] = -2;
    calc.top = 2;
    check_failure(ca_op_multiply(&calc));

    calc.stack[0] = -2;
    calc.stack[1] = CA_VALUE_MIN / 2 - 1;
    calc.top = 2;
    check_failure(ca_op_multiply(&calc));

    calc.stack[0] = -2;
    calc.stack[1] = 0 - (CA_VALUE_MAX / 2 + 1);
    calc.top = 2;
    check_failure(ca_op_multiply(&calc));
}

static void test_op_divide(void)
{
    calc.top = 0;
    check_failure(ca_op_divide(&calc));
    calc.top = 1;
    check_failure(ca_op_divide(&calc));

    calc.stack[0] = 30;
    calc.stack[1] = 7;
    calc.top = 2;
    check_success(ca_op_divide(&calc));
    check(calc.top == 1, "divide should remove two values and add the result on the stack");
    check(calc.stack[0] == 4, "divide should put the addition result on the stack");

    calc.stack[0] = 30;
    calc.stack[1] = 0;
    calc.top = 2;
    check_failure(ca_op_divide(&calc));
}

static void test_op_square_root(void)
{
    calc.top = 1;

#define CHECK_SQRT(X, Y) do {                                           \
        calc.stack[0] = X;                                              \
        check_success(ca_op_square_root(&calc));                        \
        check(calc.stack[0] == Y, "square root of %ld should be %ld", X, Y); \
    } while (0)

    CHECK_SQRT(0L, 0L);
    CHECK_SQRT(1L, 1L);
    CHECK_SQRT(2L, 1L);
    CHECK_SQRT(3L, 1L);
    CHECK_SQRT(4L, 2L);
    CHECK_SQRT(5L, 2L);
    CHECK_SQRT(6L, 2L);
    CHECK_SQRT(7L, 2L);
    CHECK_SQRT(8L, 2L);
    CHECK_SQRT(9L, 3L);
    CHECK_SQRT(63L, 7L);
    CHECK_SQRT(64L, 8L);
    CHECK_SQRT(65L, 8L);
    CHECK_SQRT(65L, 8L);
    CHECK_SQRT(1520288L, 1232L);
    CHECK_SQRT(1520289L, 1233L);
    CHECK_SQRT(1520290L, 1233L);

    calc.stack[0] = -1;
    check_failure(ca_op_square_root(&calc));
}

int main(void)
{
    test_initialize_cleanup();
    test_space_left();
    test_top();
    test_push_pop();
    test_remove();
    test_check_values();
    test_op_add();
    test_op_substract();
    test_op_multiply();
    test_op_divide();
    test_op_square_root();
    return 0;
}

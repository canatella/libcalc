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

int main(void)
{
    test_initialize_cleanup();
    test_space_left();
    test_top();
    test_push_pop();
    test_remove();
    return 0;
}

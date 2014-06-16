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
    ca_calc_t calc;
    calc.size = 10;
    calc.top = 3;
    check(ca_space_left(&calc) == 7, "ca_space_left should compute free space");
}
int main(void)
{
    test_initialize_cleanup();
    test_space_left();
    return 0;
}

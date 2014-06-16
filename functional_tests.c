#include "libcalc.h"
#include "testsuite.h"

static void test_initialize_cleanup(void)
{
    ca_calc_t calc;
    check_success(ca_initialize(&calc, 10));
    ca_cleanup(&calc);
}

static void test_push_top_pop_and_space_left(void)
{
    ca_calc_t calc;
    unsigned popped;

    check_success(ca_initialize(&calc, 10));
    check(ca_space_left(&calc) == 10, "after initialization, the space left should be the stack size");

    ca_push(&calc, 79);
    check(ca_top(&calc) == 79, "after pushing a value, the top of the stack should hold the value");
    check(ca_space_left(&calc) == 9, "after pushing a value, the space left should decrease");

    ca_push(&calc, 32);
    check(ca_top(&calc) == 32, "after pushing a value, the top of the stack should hold the value");
    check(ca_space_left(&calc) == 8, "after pushing a value, the space left should decrease");

    ca_push(&calc, 35);
    check(ca_top(&calc) == 35, "after pushing a value, the top of the stack should hold the value");
    check(ca_space_left(&calc) == 7, "after pushing a value, the space left should decrease");

    popped = ca_pop(&calc, 2);
    check(popped == 2, "pop should return the number of popped values");
    check(ca_top(&calc) == 79, "after popping values, the top should lower");
    check(ca_space_left(&calc) == 9, "after popping values, the space left should increase");

    popped = ca_pop(&calc, 0);
    check(popped == 1, "pop should return the number of popped values");
    check(ca_space_left(&calc) == 10, "after popping all values, the space left should be the stack size");

    ca_cleanup(&calc);
}
int main(void)
{
    test_initialize_cleanup();
    test_push_top_pop_and_space_left();
    return 0;
}

#include <limits.h>

#include "libcalc.h"
#include "testsuite.h"

static void test_initialize_cleanup(void)
{
    ca_calc_t calc;
    check_success(ca_initialize(&calc, 10));
    ca_cleanup(&calc);
}

static void test_push_top_pop_remove_and_space_left(void)
{
    ca_calc_t calc;
    unsigned removed;

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

    check(ca_pop(&calc) == 35, "popping a value should return the top value");
    check(ca_top(&calc) == 32, "after popping a value, the top of the stack should lower");
    check(ca_space_left(&calc) == 8, "after popping a value, the space left should increase");

    ca_push(&calc, 35);

    removed = ca_remove(&calc, 2);
    check(removed == 2, "remove should return the number of removed values");
    check(ca_top(&calc) == 79, "after popping values, the top should lower");
    check(ca_space_left(&calc) == 9, "after popping values, the space left should increase");

    removed = ca_remove(&calc, 0);
    check(removed == 1, "pop should return the number of removed values");
    check(ca_space_left(&calc) == 10, "after popping all values, the space left should be the stack size");

    ca_cleanup(&calc);
}

static void test_add(void)
{
    ca_calc_t calc;
    check_success(ca_initialize(&calc, 2));
    ca_push(&calc, 2);
    check_failure(ca_operate(&calc, CA_OP_ADD));
    ca_push(&calc, 4);
    check_success(ca_operate(&calc, CA_OP_ADD));
    check(ca_top(&calc) == 6, "adding should replace the two top element by the addition of them");
    check(ca_space_left(&calc) == 1, "adding should increase space left");
    ca_push(&calc, LONG_MAX);
    check_failure(ca_operate(&calc, CA_OP_ADD));
}

static void test_substract(void)
{
    ca_calc_t calc;
    check_success(ca_initialize(&calc, 2));
    ca_push(&calc, 2);
    check_failure(ca_operate(&calc, CA_OP_SUBSTRACT));
    ca_push(&calc, 4);
    check_success(ca_operate(&calc, CA_OP_SUBSTRACT));
    check(ca_top(&calc) == -2, "substracting should replace the two top element by the substraction of them");
    check(ca_space_left(&calc) == 1, "substracting should increase space left");
    ca_push(&calc, LONG_MAX);
    check_failure(ca_operate(&calc, CA_OP_SUBSTRACT));
    ca_remove(&calc, 0);
}

int main(void)
{
    test_initialize_cleanup();
    test_push_top_pop_remove_and_space_left();
    test_add();
    test_substract();
    return 0;
}

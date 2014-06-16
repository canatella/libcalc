#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libcalc.h"

#define STACK_SIZE 64
#define PROMPT "\n> "

static void prompt(ca_calc_t *calc)
{
    printf("stack:");
    ca_value_t *v;
    ca_stack_for_each(calc, v) {
        printf(" %ld", *v);
    }
    printf(PROMPT);
    fflush(stdout);
}

int main(void)
{
    ca_calc_t calc;
    if (ca_initialize(&calc, STACK_SIZE) < 0)
        exit(1);

    char *line = NULL;
    size_t length;

    prompt(&calc);

    while (getline(&line, &length, stdin) >= 0) {
        if (strcmp(line, "help\n") == 0) {
            printf("commands:\n"
                   "help                this help\n"
                   "+                   add\n"
                   "-                   substract\n"
                   "*                   multiply\n"
                   "/                   divide\n"
                   "%%                   modulo\n"
                   "sqrt                square root\n"
                   "<<                  left shift\n"
                   ">>                  right shift\n"
                   "pop                 pop a value from the stack\n"
                   "a number            push number on the stack\n"
                   "empty               empty the stack\n"
                   "quit                quit application\n"
                   );

        }
        else if (strcmp(line, "quit\n") == 0) {
            break;
        }
        else if (strcmp(line, "+\n") == 0) {
            ca_operate(&calc, CA_OP_ADD);
        }
        else if (strcmp(line, "-\n") == 0) {
            ca_operate(&calc, CA_OP_SUBSTRACT);
        }
        else if (strcmp(line, "*\n") == 0) {
            ca_operate(&calc, CA_OP_MULTIPLY);
        }
        else if (strcmp(line, "/\n") == 0) {
            ca_operate(&calc, CA_OP_DIVIDE);
        }
        else if (strcmp(line, "%\n") == 0) {
            ca_operate(&calc, CA_OP_MODULO);
        }
        else if (strcmp(line, "sqrt\n") == 0) {
            ca_operate(&calc, CA_OP_SQUARE_ROOT);
        }
        else if (strcmp(line, "<<\n") == 0) {
            ca_operate(&calc, CA_OP_LEFT_SHIFT);
        }
        else if (strcmp(line, ">>\n") == 0) {
            ca_operate(&calc, CA_OP_RIGHT_SHIFT);
        }
        else if (strcmp(line, "pop\n") == 0) {
            if (ca_count(&calc) > 0)
                ca_pop(&calc);
        }
        else if (strcmp(line, "empty\n") == 0) {
            ca_remove(&calc, 0);
        }
        else {
            char *endptr;
            errno = 0;
            ca_value_t value = strtol(line, &endptr, 10);
            if (errno == ERANGE) {
                fprintf(stderr, "integer is out of range.\n");
            } else if (*endptr == '\n') {
                ca_push(&calc, value);
            } else {
                fprintf(stderr, "unable to parse command.\n");
            }
        }

        prompt(&calc);
    }

    free(line);
    ca_cleanup(&calc);
}

#ifndef _TESTSUITE_H_
#define _TESTSUITE_H_

#include <stdio.h>
#include <stdlib.h>

#define check(EXPR, MSG, ...) do {                                      \
        if (!(EXPR)) {                                                  \
            fprintf(stderr, "assertion %s failed: " MSG  ", %s:%u\n", # EXPR, \
                    ## __VA_ARGS__, __FILE__, __LINE__);                \
            abort();                                                    \
        } else {                                                        \
            fprintf(stderr, "assertion %s succeeded, %s:%u\n", # EXPR,  \
                    __FILE__, __LINE__);                                \
        }                                                               \
    } while (0)

#define check_success(EXPR) check(EXPR == 0, "expected 0 for success, got -1")
#define check_failure(EXPR) check(EXPR == -1, "expected -1 for failure, got 0")

void unit_tests(void);
void functional_tests(void);

#endif /* _TESTSUITE_H_ */

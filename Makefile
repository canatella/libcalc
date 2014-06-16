CFLAGS := -Wall -Werror -g --std=gnu99

calculator: calculator.o libcalc.so
	$(CC) -o $(@) $(<) -L. -lcalc

libcalc.so: libcalc.o
	$(CC) -shared -o libcalc.so $(<)

unit_tests: unit_tests.o
	$(CC) -o $(@) -Wl,--wrap=calloc -Wl,--wrap=free $(<)

functional_tests: functional_tests.o libcalc.so
	$(CC) -o $(@) $(<) -L. -lcalc


libcalc.o: libcalc.h libcalc_priv.h
unit_tests.o: testsuite.h libcalc.h libcalc_priv.h libcalc.c
functional_tests.o: testsuite.h libcalc.h
calculator.o: libcalc.h

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $(@) $(<)

clean:
	rm -f *.o *.so

check: unit_tests functional_tests libcalc.so
	@echo running unit tests
	@LD_LIBRARY_PATH=. ./unit_tests
	@echo running functional tests
	@LD_LIBRARY_PATH=. ./functional_tests
	@echo all tests succeeded

.PHONY: clean check

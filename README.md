# Libcalc

The library is build as a shared object as it was not mentionned which
type of library to build. The test suite are the unit_tests and
functional_tests binaries. They can be run using make check.

## Building

The application assumes a unix sytem with a libc. Building should work
with a recent compiler and a simple make.

## Remarks

I could check for programming error in the library like when trying to
pop a value when the stack is empty. I kept the C way of allowing the
user to shoot himself in the foot. I used assertion so that the user
of the library knows when he has made an error.

I could add some way for the library to report which error occured.

Logging is simply done on stderr which might not be what you want for
a library.

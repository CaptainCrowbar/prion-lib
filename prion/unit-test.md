# Test Utilities #

By Ross Smith

* `#include "prion/unit-test.hpp"`

A typical test module will look something like this:

    #include "project/module.hpp"
    #include "prion/unit-test.hpp"
    TEST_MAIN;
    TEST_MODULE(project, module) {
        int answer;
        TRY(answer = 6 * 9);
        TEST_EQUAL(answer, 42);
    }

Please note that all of the macros here may evaluate their arguments more than
once.

* `#define` **`TEST_MAIN`**

Supplies the main function for a test program. This must appear at global
scope in one module if there is no user-supplied `main()` function. The main
function will check for a `UNIT` environment variable, parsing it as a PCRE
regular expression, and only the tests whose name (in _"project/module"_ form)
contains the pattern will be run. The return value from `TEST_MAIN` is zero on
success, 1 if any tests failed.

* `#define` **`TEST_MODULE`**`(project, module)`

This supplies the function signature for a module test function (see above for
typical usage).

* `#define` **`FAIL`**`(message)`

Generates a test failure, with the specified error message (a string
constant).

* `#define` **`MESSAGE`**`(message)`

Emits a message, as for `FAIL()`, but does not count this as a test failure.

* `#define` **`REQUIRE`**`(expression)`
* `#define` **`TEST`**`(expression)`

Evaluate the expression, expecting it to return a boolean value, or something
that can be explicitly converted to a boolean. The test fails if the return
value is false, or the expression throws an exception. The `REQUIRE()` macro
differs from `TEST()` in that it will return from the calling function if the
test fails; this is intended for situations where a failed test indicates that
something is so badly wrong that further testing would be pointless or
impossible.

* `#define` **`TEST_COMPARE`**`(lhs, op, rhs)`
* `#define` **`TEST_EQUAL`**`(lhs, rhs)`
* `#define` **`TEST_EQUAL_RANGE`**`(lhs, rhs)`

Evaluate the two expressions and compare the results, failing if the specified
comparison is false, or if either expression (or the comparison itself) throws
an exception. `TEST_COMPARE()` applies the given comparison operator, and is
equivalent to `TEST((lhs)op(rhs))`, but with a more informative error message
if it fails. `TEST_EQUAL()` is shorthand for `TEST_COMPARE(lhs,==,rhs)`.
`TEST_EQUAL_RANGE()` expects the expressions to evaluate to ranges, and
performs a memberwise equality comparison, failing if the ranges are different
sizes, or any of the elements fail to match.

* `#define` **`TEST_MATCH`**`(string, pattern)`
* `#define` **`TEST_MATCH_ICASE`**`(string, pattern)`

Evaluates the string expression, and searches it for the given PCRE regular
expression (supplied as a string constant), optionally case sensitive or
insensitive The test fails if the string fails to match, or an exception is
thrown.

* `#define` **`TEST_NEAR`**`(lhs, rhs)`
* `#define` **`TEST_NEAR_EPSILON`**`(lhs, rhs, tolerance)`
* `#define` **`TEST_NEAR_RANGE`**`(lhs, rhs)`
* `#define` **`TEST_NEAR_EPSILON_RANGE`**`(lhs, rhs, tolerance)`

Evaluate two floating point expressions, or two ranges of floating point
values, and compare them for approximate equality. The test fails if the
values differ by more than the tolerance (defaulting to 10<sup>-6</sup>), or
if any expression throws an exception.

* `#define` **`TEST_THROW`**`(expression, exception)`
* `#define` **`TEST_THROW_EQUAL`**`(expression, exception, message)`
* `#define` **`TEST_THROW_MATCH`**`(expression, exception, pattern)`
* `#define` **`TEST_THROW_MATCH_ICASE`**`(expression, exception, pattern)`

Evaluate the expression, expecting it to throw an exception of the specified
type (or a derived type). The test fails if the expression fails to throw an
exception, or throws an exception of the wrong type. The second and third
macros also check the exception's error message (obtained from its `what()`
method) for either an exact string match or a regular expression.

* `#define` **`TEST_TYPE`**`(type1, type2)`
* `#define` **`TEST_TYPE_OF`**`(object, type)`
* `#define` **`TEST_TYPES_OF`**`(object1, object2)`

Compare two types, failing if they are not the same. The `std::decay`
transformation is applied before comparing them.

* `#define` **`TRY`**`(expression)`

This simply evaluates the expression, ignoring any result. The test fails if
an exception is thrown.

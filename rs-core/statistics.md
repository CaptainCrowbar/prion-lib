# Statistics #

By Ross Smith

* `#include "rs-core/statistics.hpp"`

## Contents ##

* [TOC]

## Statistics class ##

* `template <typename T> class` **`Statistics`**

The accumulator class keeps running single-variable and two-variable
statistics for an arbitrary number of variables. The number of variables is
determined at runtime, from either a column count supplied to the constructor,
or the largest number of variables supplied to an update function, whichever
is larger.

Most of the statistical parameter queries take one or two indices indicating
which column or columns are being queried. Single-column statistics default to
the first column (index 0); two-column statistics default to the first two
columns.

Query functions whose names end in `_bc()` apply Bessel's correction,
adjusting sample-based statistics to be more representative of the population
from which the sample was drawn. For weighted data, the weights are assumed to
be frequency-based by default; the alternative functions with names ending in
`_bc_rel()` assume reliability-based weights instead (for unweighted data the
`*_bc()` and `*_bc_rel()` functions will return the same result).

Behaviour is undefined if any of the statistical parameters (other than
`count()` or `weight()`) are requested for an empty set, if an index is out of
bounds (greater than or equal to `columns()`), if any two-column statistics
are requested for a data set with only one column, or if any of the functions
that use Bessel's correction are called when either `count()` or `weight()` is
less than or equal to 1.

* `using Statistics::`**`scalar_type`** `= T`

The scalar type. This must be a floating point arithmetic type (checked with a
static assert).

* `Statistics::`**`Statistics`**`() noexcept`
* `explicit Statistics::`**`Statistics`**`(size_t columns)`
* `Statistics::`**`~Statistics`**`() noexcept`
* `Statistics::`**`Statistics`**`(const Statistics& s) noexcept`
* `Statistics::`**`Statistics`**`(Statistics&& s) noexcept`
* `Statistics& Statistics::`**`operator=`**`(const Statistics& s) noexcept`
* `Statistics& Statistics::`**`operator=`**`(Statistics&& s) noexcept`

Life cycle functions. Optionally a minimum number of columns can be supplied
to the constructor.

* `template <typename... Args> void Statistics::`**`operator()`**`(Args... args)`
* `template <size_t N> void Statistics::`**`i`**`(const std::array<T, N>& array)`
* `template <typename T1, typename T2> void Statistics::`**`i`**`(const std::pair<T1, T2> pair)`
* `template <typename... TS> void Statistics::`**`i`**`(const std::tuple<TS...>& tuple)`
* `template <typename... Args> void Statistics::`**`i`**`(Args... args)`
* `template <size_t N> void Statistics::`**`w`**`(T wt, const std::array<T, N>& array)`
* `template <typename T1, typename T2> void Statistics::`**`w`**`(T wt, const std::pair<T1, T2> pair)`
* `template <typename... TS> void Statistics::`**`w`**`(T wt, const std::tuple<TS...>& tuple)`
* `template <typename... Args> void Statistics::`**`w`**`(T wt, Args... args)`

Add a new data point. The `i()` functions add an unweighted data point
(equivalent to a weight of 1); the `w()` functions add a weighted data point.
The function call operator is equivalent to `i()`. The column count will be
expanded if necessary to match the number of values in the update. If the
column count is expanded, missing data in any previous updates is assumed to
consist of zeros; if this call supplies less than the existing number of
columns, the missing values are assumed to be zero. Behaviour is undefined if
the weight is negative.

* `size_t Statistics::`**`columns`**`() const noexcept`

Returns the current number of columns.

* `size_t Statistics::`**`count`**`() const noexcept`

Returns the number of data points entered so far.

* `bool Statistics::`**`empty`**`() const noexcept`

True if no data has been entered yet (equivalent to `count()==0`).

* `T Statistics::`**`weight`**`() const noexcept`

Returns the total of all weights so far. For unweighted data this will be
equal to `count()`.

* `T Statistics::`**`min`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`max`**`(size_t i = 0) const noexcept`

Maximum and minimum values of each column.

* `T Statistics::`**`sum`**`(size_t i = 0) const noexcept`

Weighted sum of each column.

* `T Statistics::`**`mean`**`(size_t i = 0) const noexcept`

Weighted mean of each column.

* `T Statistics::`**`variance`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`variance_bc`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`variance_bc_rel`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`sd`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`sd_bc`**`(size_t i = 0) const noexcept`
* `T Statistics::`**`sd_bc_rel`**`(size_t i = 0) const noexcept`

Standard deviation of each column.

* `T Statistics::`**`covariance`**`() const noexcept`
* `T Statistics::`**`covariance`**`(size_t i, size_t j) const noexcept`
* `T Statistics::`**`covariance_bc`**`() const noexcept`
* `T Statistics::`**`covariance_bc`**`(size_t i, size_t j) const noexcept`
* `T Statistics::`**`covariance_bc_rel`**`() const noexcept`
* `T Statistics::`**`covariance_bc_rel`**`(size_t i, size_t j) const noexcept`
* `T Statistics::`**`correlation`**`() const noexcept`
* `T Statistics::`**`correlation`**`(size_t i, size_t j) const noexcept`

Covariance and correlation (Pearson's product moment correlation coefficient)
of each pair of columns.

* `std::pair<T, T> Statistics::`**`linear`**`() const noexcept`
* `std::pair<T, T> Statistics::`**`linear`**`(size_t i, size_t j) const noexcept`

The coefficients of the least squares linear regression equation for each pair
of columns (the coefficients _(a,b)_ in _y&#x302;=ax+b_ or _x&#x302;=ay+b_).

* `void Statistics::`**`clear`**`() noexcept`
* `void Statistics::`**`clear`**`(size_t columns) noexcept`

Reset the accumulator to an empty set, optionally setting an initial column
count.

* `void Statistics::`**`swap`**`(Statistics& s) noexcept`
* `void` **`swap`**`(Statistics& s1, Statistics& s2) noexcept`

Swap two objects.

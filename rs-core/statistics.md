# Statistics #

By Ross Smith

* `#include "rs-core/statistics.hpp"`

## Contents ##

* [TOC]

## Statistics class ##

* `template <typename T = double> class` **`Statistics`**

Accumulator class. This holds bivariate statistics for a set of _(x,y)_ pairs,
but can also be used to obtain univariate statistics for a single data set.

* `using Statistics::`**`scalar_type`** `= T`

The scalar type. This must be a floating point arithmetic type (checked with a
static assert).

* `Statistics::`**`Statistics`**`() noexcept`
* `Statistics::`**`~Statistics`**`() noexcept`
* `Statistics::`**`Statistics`**`(const Statistics& s) noexcept`
* `Statistics::`**`Statistics`**`(Statistics&& s) noexcept`
* `Statistics& Statistics::`**`operator=`**`(const Statistics& s) noexcept`
* `Statistics& Statistics::`**`operator=`**`(Statistics&& s) noexcept`

Life cycle functions.

* `void Statistics::`**`add`**`(T x, T y = 0) noexcept`
* `void Statistics::`**`add`**`(const std::pair<T, T>& xy) noexcept`
* `template <typename InputRange> void Statistics::`**`append`**`(const InputRange& r)`

Add one or more data points, either as univariate `x` values or bivariate
_(x,y)_ pairs. The `append()` function can read a vector-like range of values
or a map-like range of pairs.

* `void Statistics::`**`clear`**`() noexcept`

Resets all of the internal accumulators.

* `bool Statistics::`**`empty`**`() const noexcept`

True if the data set is empty (`num()==0`).

* `size_t Statistics::`**`num`**`() const noexcept`

Returns the number of data points entered.

* `T Statistics::`**`min`**`() const noexcept`
* `T Statistics::`**`min_x`**`() const noexcept`
* `T Statistics::`**`min_y`**`() const noexcept`
* `T Statistics::`**`max`**`() const noexcept`
* `T Statistics::`**`max_x`**`() const noexcept`
* `T Statistics::`**`max_y`**`() const noexcept`

Minimum and maximum values of `x` and `y`. These will return zero if the data
set is empty. The functions with no suffix return the univariate statistics
(the same values as the `*_x()` functions).

* `T Statistics::`**`mean`**`() const noexcept`
* `T Statistics::`**`mean_x`**`() const noexcept`
* `T Statistics::`**`mean_y`**`() const noexcept`

Mean values of `x` and `y`. These will return zero if the data set is empty.
The function with no suffix returns the univariate statistics (the same value
as `mean_x()`).

* `T Statistics::`**`stdevp`**`() const noexcept`
* `T Statistics::`**`stdevp_x`**`() const noexcept`
* `T Statistics::`**`stdevp_y`**`() const noexcept`
* `T Statistics::`**`stdevs`**`() const noexcept`
* `T Statistics::`**`stdevs_x`**`() const noexcept`
* `T Statistics::`**`stdevs_y`**`() const noexcept`

Standard deviations of `x` and `y`. The `stdevp*()` functions return the
population standard deviation (assuming the supplied data is the complete
population); the `stdevs*()` functions return the sample standard deviation
(assuming the supplied data is a sample of a large population). All of these
will return zero if the data set is empty; the sample functions will return
zero if less than two data points are available. The functions with no suffix
return the univariate statistics (the same values as the `*_x()` functions).

* `T Statistics::`**`correlation`**`() const noexcept`

Returns Pearson's product moment correlation coefficient. This will return
zero if less than two data points are available, or if the data set is
degenerate and no correlation can be calculated.

* `std::pair<T, T> Statistics::`**`linear`**`() const noexcept`

Returns the coefficients of the least squares linear regression equation (the
coefficients _(a,b)_ in _y&#x302;=ax+b_). If the data set is empty, this will
return _(0,0)_; if only one data point has been supplied, or if the data set
is degenerate and no regression can be calculated, it will return
_(0,y&#x304;)_.

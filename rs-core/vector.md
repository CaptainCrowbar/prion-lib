# Vectors, Matrices, and Quaternions #

By Ross Smith

* `#include "rs-core/vector.hpp"`

## Contents ##

[TOC]

## Vector ##

* `template <typename T, size_t N> class` **`Vector`**

An `N`-dimensional vector type. `T` must be an arithmetic type; `N` must be a
positive integer.

`Vector` supports mixed mode arithmetic, in which the operands of a binary
operator (or in some cases a named function) may have different value types.
In these cases the return type is given as `CT`, meaning the
`std::common_type` of the operands' value types.

* `using` **`Int[1-4]`** `= Vector<int, [1-4]>`
* `using` **`Int8_[1-4]`** `= Vector<int8_t, [1-4]>`
* `using` **`Int16_[1-4]`** `= Vector<int16_t, [1-4]>`
* `using` **`Int32_[1-4]`** `= Vector<int32_t, [1-4]>`
* `using` **`Int64_[1-4]`** `= Vector<int64_t, [1-4]>`
* `using` **`Uint8_[1-4]`** `= Vector<uint8_t, [1-4]>`
* `using` **`Uint16_[1-4]`** `= Vector<uint16_t, [1-4]>`
* `using` **`Uint32_[1-4]`** `= Vector<uint32_t, [1-4]>`
* `using` **`Uint64_[1-4]`** `= Vector<uint64_t, [1-4]>`
* `using` **`Ptrdiff[1-4]`** `= Vector<ptrdiff_t, [1-4]>`
* `using` **`Size[1-4]`** `= Vector<size_t, [1-4]>`
* `using` **`Float[1-4]`** `= Vector<float, [1-4]>`
* `using` **`Double[1-4]`** `= Vector<double, [1-4]>`
* `using` **`Ldouble[1-4]`** `= Vector<long double, [1-4]>`

Named instantiations (the shorthand used here indicates instantiations for 1
to 4 dimensions).

* `using Vector::`**`value_type`** `= T`

Member types.

* `static constexpr size_t Vector::`**`dim`** `= N`

Member constants.

* `Vector::`**`Vector`**`() noexcept`
* `template <typename T2> explicit Vector::`**`Vector`**`(T2 t) noexcept`
* `template <typename... Args> Vector::`**`Vector`**`(Args... args) noexcept`
* `template <typename T2> explicit Vector::`**`Vector`**`(const T2* ptr) noexcept`
* `template <typename T2> Vector::`**`Vector`**`(const Vector<T2, N>& v) noexcept`
* `Vector::`**`~Vector`**`() noexcept`
* `Vector::`**`Vector`**`(const Vector& v) noexcept`
* `Vector::`**`Vector`**`(Vector&& v) noexcept`
* `Vector& Vector::`**`operator=`**`(const Vector& v) noexcept`
* `Vector& Vector::`**`operator=`**`(Vector&& v) noexcept`
* `template <typename T2> Vector& Vector::`**`operator=`**`(const Vector<T2, N>& v) noexcept`

Life cycle functions. The default constructor sets all elements to zero (the
same value as the static `zero()` function); the second constructor sets all
elements to the given value; the third takes an explicit list of elements; the
fourth copies `N` elements starting from the source pointer (behaviour is
undefined if the pointer is null); the fifth copies the elements of the
argument vector. The constructors that take a different type depend on the
existence of an implicit conversion from `T2` to `T`.

* `T& Vector::`**`operator[]`**`(size_t i) noexcept`
* `const T& Vector::`**`operator[]`**`(size_t i) const noexcept`
* `T& Vector::`**`at`**`(size_t i)`
* `const T& Vector::`**`at`**`(size_t i) const`
* `T& Vector::`**`x`**`() noexcept`
* `const T& Vector::`**`x`**`() const noexcept`
* `T& Vector::`**`y`**`() noexcept`
* `const T& Vector::`**`y`**`() const noexcept`
* `T& Vector::`**`z`**`() noexcept`
* `const T& Vector::`**`z`**`() const noexcept`
* `T& Vector::`**`w`**`() noexcept`
* `const T& Vector::`**`w`**`() const noexcept`
* `T* Vector::`**`begin`**`() noexcept`
* `const T* Vector::`**`begin`**`() const noexcept`
* `T* Vector::`**`end`**`() noexcept`
* `const T* Vector::`**`end`**`() const noexcept`

Data access functions. If the index is out of range, behaviour is undefined
for `operator[]`; `at()` throws `std::out_of_range`. The `x()`, `y()`, `z()`,
and `w()` methods yield the first four elements. Behaviour is undefined if
`y()`, `z()`, or `w()` is called on a vector with too few elements.

* `template <size_t N2> Vector<T, N2 - 1> Vector::`**`operator[]`**`(const char (&str)[N2]) const`

Swizzle operator. This returns a vector made from the components of the
original vector indicated by letters in the string literal argument; for
example, `v["zyx"]` will return `{v.z(),v.y(),v.x()}`. Behaviour is undefined
if the string contains any characters other than `'x'`, `'y'`, `'z'`, or
`'w'`, or refers to nonexistent components if the original vector had less
than four components. The output vector has the same number of components as
there are letters in the string (the length is `N-1` to remove the implicit
null terminator).

* `Vector Vector::`**`operator+`**`() const noexcept`
* `Vector Vector::`**`operator-`**`() const noexcept`

Unary arithmetic operators.

* `template <typename T1, typename T2> Vector<CT, 3>` **`operator^`**`(const Vector<T1, 3>& lhs, const Vector<T2, 3>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> CT` **`operator%`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2> Vector<CT, 3>` **`cross`**`(const Vector<T1, 3>& lhs, const Vector<T2, 3>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> CT` **`dot`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`

Vector cross (`^`) and dot (`%`) product operators, and equivalent functions.

* `template <typename T2> Vector& Vector::`**`operator+=`**`(const Vector<T2, N>& rhs) noexcept`
* `template <typename T2> Vector& Vector::`**`operator-=`**`(const Vector<T2, N>& rhs) noexcept`
* `template <typename T2> Vector& Vector::`**`operator*=`**`(const Vector<T2, N>& rhs) noexcept`
* `template <typename T2> Vector& Vector::`**`operator/=`**`(const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator+`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator-`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator*`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator/`**`(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`

Element-wise arithmetic operators. In each case the corresponding operation
between `T1` and `T2` must be defined. Behaviour is undefined on division by
zero.

* `template <typename T2> Vector& Vector::`**`operator*=`**`(T2 rhs) noexcept`
* `template <typename T2> Vector& Vector::`**`operator/=`**`(T2 rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator*`**`(const Vector<T1, N>& lhs, T2 rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator*`**`(T1 lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<CT, N>` **`operator/`**`(const Vector<T1, N>& lhs, T2 rhs) noexcept`

Vector-scalar arithmetic operators. In each case the corresponding operation
between `T1` and `T2` must be defined. Behaviour is undefined on division by
zero.

* `T Vector::`**`angle`**`(const Vector& v) const noexcept`

Returns the angle between two vectors (in the range _[0,&pi;]_). This will
return zero if either vector is null. Requires `T` to be a floating point
type.

* `Vector Vector::`**`dir`**`() const noexcept`

Returns a unit vector parallel to this vector (or an approximation to it,
given the limits of floating point arithmetic). This requires `T` to be a
floating point type.

* `bool Vector::`**`is_null`**`() const noexcept`

True if the vector is null.

* `Vector Vector::`**`project`**`(const Vector& v) const noexcept`
* `Vector Vector::`**`reject`**`(const Vector& v) const noexcept`

These return the projection and rejection of `*this` onto `v` (the components
of `*this` parallel and orthogonal to `v`). If `v` is null, `project()`
returns null and `reject()` returns `*this`.

* `T Vector::`**`r`**`() const noexcept`
* `T Vector::`**`r2`**`() const noexcept`

These return the length of the vector, or its square. The `r()` function
requires `T` to be a floating point type.

* `constexpr size_t Vector::`**`size`**`() const noexcept`

Returns `N`.

* `static Vector Vector::`**`unit`**`(size_t i) noexcept`

Returns a unit vector along the given axis. Behaviour is undefined if the
index is out of range.

* `static Vector Vector::`**`zero`**`() noexcept`

Returns the zero vector (the same value as the default constructor).

* `bool` **`operator==`**`(const Vector& lhs, const Vector& rhs) noexcept`
* `bool` **`operator!=`**`(const Vector& lhs, const Vector& rhs) noexcept`

Comparison operators.

* `std::ostream&` **`operator<<`**`(std::ostream& out, const Vector& v)`
* `Ustring` **`to_str`**`(const Vector& v)`

Simple string formatting; this simply writes the vector's elements in their
default format, enclosed in square brackets.

* `template <typename T1, typename T2, typename T3> Vector<T1, N>` **`clamp`**`(Vector<T1, N> x, Vector<T2, N> min, Vector<T3, N> max) noexcept`
* `Vector` **`min`**`(Vector x, Vector y) noexcept`
* `Vector` **`max`**`(Vector x, Vector y) noexcept`
* `std::pair<Vector, Vector>` **`minmax`**`(Vector x, Vector y) noexcept`

Vector algorithms. These apply the corresponding scalar operations to each
member.

* `template <typename T, size_t N> struct std::`**`hash`**`<Vector<T, N>>`

Hash function.

* `template <typename T, size_t N> struct std::`**`greater`**`<Vector<T, N>>`
* `template <typename T, size_t N> struct std::`**`less`**`<Vector<T, N>>`

`Vector` has no natural ordering and therefore no less-than operator, but
these specializations are provided to allow `Vector` to be used as the key in
ordered containers.

## Matrix ##

* `enum class` **`MatrixLayout`**
    * **`column`** `= 'C'`
    * **`row`** `= 'R'`

Flags indicating the internal layout of a matrix.

* `template <typename T, size_t N, MatrixLayout L = MatrixLayout::column> class` **`Matrix`**

An `N`&times;`N` square matrix type. `T` must be an arithmetic type; `N` must
be a positive integer; `L` must be one of the `MatrixLayout` enumeration.

For simplicity of presentation, most of the matrix operations described below
are documented as though there were only two matrix parameters, the element
type and the size; the layout is left out to avoid complicating the
documentation with too many multiple overloads. Except where the documentation
explicitly says otherwise, all matrix operations work the same for both
layouts. Operations that take two matrices will work on pairs of different
layout. When a dissimilar pair is passed to an operation that returns another
matrix (e.g. adding or multiplying a column matrix with a row matrix), the
returned matrix has the same layout as the first argument.

* `using` **`Float[2-4]x[2-4]`** `= Matrix<float, [2-4]>`
* `using` **`Float[2-4]x[2-4]c`** `= Matrix<float, [2-4], MatrixLayout::column>`
* `using` **`Float[2-4]x[2-4]r`** `= Matrix<float, [2-4], MatrixLayout::row>`
* `using` **`Double[2-4]x[2-4]`** `= Matrix<double, [2-4]>`
* `using` **`Double[2-4]x[2-4]c`** `= Matrix<double, [2-4], MatrixLayout::column>`
* `using` **`Double[2-4]x[2-4]r`** `= Matrix<double, [2-4], MatrixLayout::row>`
* `using` **`Ldouble[2-4]x[2-4]`** `= Matrix<long double, [2-4]>`
* `using` **`Ldouble[2-4]x[2-4]c`** `= Matrix<long double, [2-4], MatrixLayout::column>`
* `using` **`Ldouble[2-4]x[2-4]r`** `= Matrix<long double, [2-4], MatrixLayout::row>`

Named instantiations.

* `using Matrix::`**`value_type`** `= T`
* `using Matrix::`**`vector_type`** `= Vector<T, N>`

Member types.

* `static constexpr size_t Matrix::`**`dim`** `= N`
* `static constexpr size_t Matrix::`**`cells`** `= N * N`
* `static constexpr MatrixLayout Matrix::`**`layout`** `= L`

Member constants.

* `Matrix::`**`Matrix`**`() noexcept`
* `template <typename T2> explicit Matrix::`**`Matrix`**`(T2 t) noexcept`
* `template <typename T2, typename T3> Matrix::`**`Matrix`**`(T2 lead, T3 other) noexcept`
* `template <typename T2> Matrix::`**`Matrix`**`(std::initializer_list<T2> ts) noexcept`
* `Matrix::`**`Matrix`**`(const Matrix& m) noexcept`
* `Matrix::`**`Matrix`**`(Matrix&& m) noexcept`
* `Matrix::`**`~Matrix`**`() noexcept`
* `Matrix& Matrix::`**`operator=`**`(const Matrix& m) noexcept`
* `Matrix& Matrix::`**`operator=`**`(Matrix&& m) noexcept`
* `template <typename T2> Matrix& Matrix::`**`operator=`**`(std::initializer_list<T2> ts) noexcept`

Life cycle functions. The default constructor sets all elements to zero (the
same result as the static `zero()` function); the second constructor sets all
elements to the given value (performing implicit conversion if necessary); the
third constructor sets the leading diagonal to one value and the remaining
elements to another; the fourth takes an explicit list of elements.

The constructors and assignment operators that take a list of elements will
assign elements in the order implied by the matrix's layout flag. If an
initializer list contains too few elements, the missing elements will be set
to zero; if it contains too many, the excess elements are ignored.

A matrix can be constructed or assigned from one of the opposite layout.

* `T& Matrix::`**`operator()`**`(size_t r, size_t c) noexcept`
* `T Matrix::`**`operator()`**`(size_t r, size_t c) const noexcept`
* `T& Matrix::`**`at`**`(size_t r, size_t c)`
* `T Matrix::`**`at`**`(size_t r, size_t c) const`
* `T* Matrix::`**`begin`**`() noexcept`
* `const T* Matrix::`**`begin`**`() const noexcept`
* `T* Matrix::`**`end`**`() noexcept`
* `const T* Matrix::`**`end`**`() const noexcept`

Element reference functions. If the row or column is out of range, behaviour
is undefined for `operator()`; `at()` throws `std::out_of_range`. Iterating
over the elements will visit them in the order implied by the matrix's layout
flag.

* `Matrix Matrix::`**`operator+`**`() const noexcept`
* `Matrix Matrix::`**`operator-`**`() const noexcept`

Unary arithmetic operators.

* `Matrix& Matrix::`**`operator+=`**`(const Matrix& rhs) noexcept`
* `Matrix& Matrix::`**`operator-=`**`(const Matrix& rhs) noexcept`
* `Matrix` **`operator+`**`(const Matrix& lhs, const Matrix& rhs) noexcept`
* `Matrix` **`operator-`**`(const Matrix& lhs, const Matrix& rhs) noexcept`
* `Matrix` **`operator*`**`(const Matrix& lhs, const Matrix& rhs) noexcept`

Matrix arithmetic operators. No `operator*=` is provided because matrices are
normally pre-multiplied.

* `template <typename T1, typename T2, size_t N> Vector<T2, N>` **`operator*`**`(const Matrix<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Vector<T1, N>` **`operator*`**`(const Vector<T1, N>& lhs, const Matrix<T2, N>& rhs) noexcept`

Matrix-vector arithmetic operators.

* `template <typename T2> Matrix& Matrix::`**`operator*=`**`(T2 rhs) noexcept`
* `template <typename T2> Matrix& Matrix::`**`operator/=`**`(T2 rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Matrix<T1, N>` **`operator*`**`(const Matrix<T1, N>& lhs, T2 rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Matrix<T2, N>` **`operator*`**`(T1 lhs, const Matrix<T2, N>& rhs) noexcept`
* `template <typename T1, typename T2, size_t N> Matrix<T1, N>` **`operator/`**`(const Matrix<T1, N>& lhs, T2 rhs) noexcept`

Matrix-scalar arithmetic operators.

* `vector_type Matrix::`**`column`**`(size_t c) const noexcept`
* `vector_type Matrix::`**`row`**`(size_t r) const noexcept`

Return the selected column or row as a vector. Behaviour is undefined if the
index is out of bounds.

* `T Matrix::`**`det`**`() const noexcept`

Returns the determinant of the matrix. This is currently only implemented for
`N<=4`.

* `Matrix Matrix::`**`inverse`**`() const noexcept`

Calculate the inverse of a matrix. This requires `T` to be a floating point
type. Behaviour is undefined if the matrix is singular. This is currently only
implemented for `N<=4`.

* `void Matrix::`**`set_column`**`(size_t c, vector_type v) noexcept`
* `void Matrix::`**`set_row`**`(size_t r, vector_type v) noexcept`

Replace a column or row with the vector. Behaviour is undefined if the index
is out of bounds.

* `constexpr size_t Matrix::`**`size`**`() const noexcept`

Returns `N*N`.

* `Matrix Matrix::`**`swap_columns`**`(size_t c1, size_t c2) const noexcept`
* `Matrix Matrix::`**`swap_rows`**`(size_t r1, size_t r2) const noexcept`

Returns a matrix with two columns or rows swapped. A copy of the original
matrix is returned if the two arguments are the same. Behaviour is undefined
if either index is out of range.

* `Matrix Matrix::`**`transpose`**`() const noexcept`

Calculate the transpose of a matrix.

* `static Matrix Matrix::`**`from_array`**`(const T* ptr) noexcept`

Builds a matrix by copying elements from the array supplied. Cells are filled
in the order implied by the matrix layout. Behaviour is undefined if the
pointer is null or the array does not have enough elements to fill the matrix.

* `template <typename... Args> static Matrix Matrix::`**`from_columns`**`(Args... args) noexcept`
* `template <typename... Args> static Matrix Matrix::`**`from_rows`**`(Args... args) noexcept`

Builds a matrix from an explicitly supplied list of elements. These are
similar to the initializer list constructor, except that the elements are
filled in column or row order regardless of the matrix layout. The same rules
are followed if the length of the argument list does not match the size of the
matrix.

* `static Matrix Matrix::`**`identity`**`() noexcept`
* `static Matrix Matrix::`**`zero`**`() noexcept`

Return the identity or zero matrix.

* `bool` **`operator==`**`(const Matrix& lhs, const Matrix& rhs) noexcept`
* `bool` **`operator!=`**`(const Matrix& lhs, const Matrix& rhs) noexcept`

Comparison operators.

* `std::ostream&` **`operator<<`**`(std::ostream& out, const Matrix& m)`
* `Ustring` **`to_str`**`(const Matrix& m)`

Simple string formatting; this writes the matrix by row in nested array form.

* `template <typename T, size_t N, RS::MatrixLayout L> struct std::`**`hash`**`<Matrix<T, N, L>>`

Hash function.

## Quaternion ##

* `template <typename T> class` **`Quaternion`**

A quaternion type based on the scalar type `T`, which must be a floating point
arithmetic type.

* `using` **`Qfloat`** `= Quaternion<float>`
* `using` **`Qdouble`** `= Quaternion<double>`
* `using` **`Qldouble`** `= Quaternion<long double>`

Named instantiations.

* `using Quaternion::`**`value_type`** `= T`

Member types.

* `Quaternion::`**`Quaternion`**`() noexcept`
* `Quaternion::`**`Quaternion`**`(T a) noexcept`
* `Quaternion::`**`Quaternion`**`(T a, T b, T c, T d) noexcept`
* `Quaternion::`**`Quaternion`**`(T a, const Vector<T, 3>& bcd) noexcept`
* `Quaternion::`**`Quaternion`**`(const Quaternion& q) noexcept`
* `Quaternion::`**`Quaternion`**`(Quaternion&& q) noexcept`
* `Quaternion::`**`~Quaternion`**`() noexcept`
* `Quaternion& Quaternion::`**`operator=`**`(const Quaternion& q) noexcept`
* `Quaternion& Quaternion::`**`operator=`**`(Quaternion&& q) noexcept`

Life cycle functions. The default constructor sets all components to zero; the
second constructor creates a scalar quaternion; the third and fourth
constructors assemble a quaternion from four components supplied explicitly or
as scalar and vector parts.

* `T& Quaternion::`**`a`**`() noexcept`
* `const T& Quaternion::`**`a`**`() const noexcept`
* `T& Quaternion::`**`b`**`() noexcept`
* `const T& Quaternion::`**`b`**`() const noexcept`
* `T& Quaternion::`**`c`**`() noexcept`
* `const T& Quaternion::`**`c`**`() const noexcept`
* `T& Quaternion::`**`d`**`() noexcept`
* `const T& Quaternion::`**`d`**`() const noexcept`
* `T* Quaternion::`**`begin`**`() noexcept`
* `const T* Quaternion::`**`begin`**`() const noexcept`
* `T* Quaternion::`**`end`**`() noexcept`
* `const T* Quaternion::`**`end`**`() const noexcept`
* `T& Quaternion::`**`operator[]`**`(size_t i) noexcept`
* `const T& Quaternion::`**`operator[]`**`(size_t i) const noexcept`

Element access functions. Behaviour is undefined if the index in `operator[]`
is out of range (greater than 3).

* `Quaternion Quaternion::`**`operator+`**`() const noexcept`
* `Quaternion Quaternion::`**`operator-`**`() const noexcept`

Unary arithmetic operators.

* `Quaternion& Quaternion::`**`operator+=`**`(const Quaternion& rhs) noexcept`
* `Quaternion& Quaternion::`**`operator-=`**`(const Quaternion& rhs) noexcept`
* `Quaternion& Quaternion::`**`operator*=`**`(const Quaternion& rhs) noexcept`
* `Quaternion` **`operator+`**`(const Quaternion& lhs, const Quaternion& rhs) noexcept`
* `Quaternion` **`operator-`**`(const Quaternion& lhs, const Quaternion& rhs) noexcept`
* `Quaternion` **`operator*`**`(const Quaternion& lhs, const Quaternion& rhs) noexcept`

Quaternion arithmetic operators.

* `template <typename T2> Quaternion& Quaternion::`**`operator*=`**`(T2 rhs) noexcept`
* `template <typename T2> Quaternion& Quaternion::`**`operator/=`**`(T2 rhs) noexcept`
* `template <typename T1, typename T2> Quaternion<T1>` **`operator*`**`(const Quaternion<T1>& lhs, T2 rhs) noexcept`
* `template <typename T1, typename T2> Quaternion<T2>` **`operator*`**`(T1 lhs, const Quaternion<T2>& rhs) noexcept`
* `template <typename T1, typename T2> Quaternion<T1>` **`operator/`**`(const Quaternion<T1>& lhs, T2 rhs) noexcept`

Quaternion-scalar arithmetic operators.

* `bool` **`operator==`**`(const Quaternion& lhs, const Quaternion& rhs) noexcept`
* `bool` **`operator!=`**`(const Quaternion& lhs, const Quaternion& rhs) noexcept`

Comparison operators.

* `Quaternion Quaternion::`**`conj`**`() const noexcept`
* `Quaternion Quaternion::`**`conj`**`(const Quaternion& p) const noexcept`

The first version returns the conjugate quaternion. The second version
performs the conjugation of `p` by `*this`, returning
<code>qpq<sup>-1</sup></code> (where `q=*this`).

* `T Quaternion::`**`norm`**`() const noexcept`
* `T Quaternion::`**`norm2`**`() const noexcept`

Returns the quaternion's norm, or its square.

* `Quaternion Quaternion::`**`recip`**`() const noexcept`

Returns the reciprocal of the quaternion. Behaviour is undefined if the
quaternion is zero.

* `T Quaternion::`**`s_part`**`() const noexcept`
* `Vector<T, 3> Quaternion::`**`v_part`**`() const noexcept`

Return the scalar and vector parts of the quaternion.

* `Quaternion Quaternion::`**`versor`**`() const noexcept`

Returns the quaternion's versor (unit quaternion). Behaviour is undefined if
the quaternion is zero.

* `Vector<T, 4> Quaternion::`**`to_vector`**`() const noexcept`
* `static Quaternion` **`from_vector`**`(const Vector<T, 4>& v) noexcept`

Component-wise conversions between a quaternion and a 4-vector,

* `std::ostream&` **`operator<<`**`(std::ostream& out, const Quaternion& q)`
* `Ustring` **`to_str`**`(const Quaternion& q)`

Simple string formatting; this simply writes the quaternion in the same format
as a 4-vector.

* `template <typename T> struct std::`**`hash`**`<Quaternion<T>>`

Hash function.

## Transformations ##

### Coordinate transformations ###

* `template <typename T> Vector<T, 2>` **`cartesian_to_polar`**`(const Vector<T, 2>& xy) noexcept`
* `template <typename T> Vector<T, 2>` **`polar_to_cartesian`**`(const Vector<T, 2>& rt) noexcept`
* `template <typename T> Vector<T, 3>` **`cartesian_to_cylindrical`**`(const Vector<T, 3>& xyz) noexcept`
* `template <typename T> Vector<T, 3>` **`cartesian_to_spherical`**`(const Vector<T, 3>& xyz) noexcept`
* `template <typename T> Vector<T, 3>` **`cylindrical_to_cartesian`**`(const Vector<T, 3>& rpz) noexcept`
* `template <typename T> Vector<T, 3>` **`cylindrical_to_spherical`**`(const Vector<T, 3>& rpz) noexcept`
* `template <typename T> Vector<T, 3>` **`spherical_to_cartesian`**`(const Vector<T, 3>& rpt) noexcept`
* `template <typename T> Vector<T, 3>` **`spherical_to_cylindrical`**`(const Vector<T, 3>& rpt) noexcept`

Transformations between coordinate systems in two or three dimensions. These
require `T` to be a floating point type.

### Projective geometry ###

* `template <typename T> Vector<T, 4>` **`make4`**`(const Vector<T, 3>& v, T w) noexcept`
* `template <typename T> Vector<T, 4>` **`point4`**`(const Vector<T, 3>& v) noexcept`
* `template <typename T> Vector<T, 4>` **`norm4`**`(const Vector<T, 3>& v) noexcept`

Convert a 3-vector to a 4-vector. The `point4()` function sets `w` to 1;
`norm4()` sets it to zero.

* `template <typename T> Vector<T, 3>` **`point3`**`(const Vector<T, 4>& v) noexcept`
* `template <typename T> Vector<T, 3>` **`norm3`**`(const Vector<T, 4>& v) noexcept`

Convert a 4-vector to a 3-vector. The `point3()` function divides the
3-coordinates by `w` (unless `w` is zero); `norm3()` just discards `w` and
returns the truncated vector.

* `template <typename T, int L> Matrix<T, 4, L>` **`make_transform`**`(const Matrix<T, 3, L>& m, const Vector<T, 3>& v) noexcept`

Composes a 4&times;4 projective transformation matrix from a 3&times;3 matrix
and a translation vector.

    (a b c)             (a b c x)
    (d e f), (x y z) => (d e f y)
    (g h i)             (g h i z)
                        (0 0 0 1)

* `template <typename T, int L> Matrix<T, 4, L>` **`normal_transform`**`(const Matrix<T, 4, L>& m) noexcept`

Converts a point transform to a normal transform (returns the transpose of the
inverse of the matrix).

### Primitive transformations ###

* `template <typename T> Matrix<T, 3>` **`rotate3`**`(T angle, size_t index) noexcept`
* `template <typename T> Matrix<T, 4>` **`rotate4`**`(T angle, size_t index) noexcept`

Generate a rotation by the given angle as a 3D or projective matrix. The index
indicates the axis of rotation (0-2); behaviour is undefined if this is out of
range.

* `template <typename T> Matrix<T, 3>` **`rotate3`**`(T angle, const Vector<T, 3>& axis) noexcept`
* `template <typename T> Matrix<T, 4>` **`rotate4`**`(T angle, const Vector<T, 3>& axis) noexcept`

Generate a rotation by the given angle, about the given axis, as a 3D or
projective matrix. They will return an identity matrix if either argument is
null.

* `template <typename T> Matrix<T, 3>` **`scale3`**`(T t) noexcept`
* `template <typename T> Matrix<T, 3>` **`scale3`**`(const Vector<T, 3>& v) noexcept`
* `template <typename T> Matrix<T, 4>` **`scale4`**`(T t) noexcept`
* `template <typename T> Matrix<T, 4>` **`scale4`**`(const Vector<T, 3>& v) noexcept`

Generate a proportional or triaxial scaling transformation as a 3D or
projective matrix.

* `template <typename T> Matrix<T, 4>` **`translate4`**`(const Vector<T, 3>& v) noexcept`

Generates a translation as a projective matrix (equivalent to
`make_transform(Matrix<T,3>::identity(),v)`).

### Quaternion transformations ###

* `template <typename T> Vector<T, 3>` **`rotate`**`(const Quaternion<T>& q, const Vector<T, 3>& v) noexcept`

Apply the rotation represented by `q` to the vector `v`.

* `template <typename T> Matrix<T, 3>` **`rotate3`**`(const Quaternion<T>& q) noexcept`
* `template <typename T> Matrix<T, 4>` **`rotate4`**`(const Quaternion<T>& q) noexcept`

Convert a quaternion into a 3-matrix or projective matrix representing the
same rotation.

* `template <typename T> Quaternion<T>` **`rotateq`**`(T angle, const Vector<T, 3>& axis) noexcept`

Generate the quaternion corresponding to a rotation by the given angle, about
the given axis.

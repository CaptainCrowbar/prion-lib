# Tagged Type #

By Ross Smith

* `#include "rs-core/marked.hpp"`

## Contents ##

[TOC]

## Marked class template ##

* `enum class` **`Mark`**`: uint32_t`
* `template <typename T, typename ID, Mark Flags = Mark::none, typename Check = void> class` **`Marked`**
    * `using Marked::`**`value_type`** `= T`
    * `using Marked::`**`id_type`** `= ID`
    * `using Marked::`**`check_type`** `= Check`
    * `static constexpr Mark Marked::`**`flags`** `= Flags`
    * `[ Marked::`**`Marked`**`() ]`
    * `[ Marked::`**`Marked`**`(const Marked& m) ]`
    * `[ Marked::`**`Marked`**`(Marked&& m) noexcept ]`
    * `[ [explicit] Marked::`**`Marked`**`(const T& t) ]`
    * `[ [explicit] Marked::`**`Marked`**`(T&& t) noexcept ]`
    * `Marked::`**`~Marked`**`() noexcept`
    * `[ Marked& Marked::`**`operator=`**`(const Marked& m) ]`
    * `[ Marked& Marked::`**`operator=`**`(Marked&& m) noexcept ]`
    * `[ Marked& Marked::`**`operator=`**`(const T& t) ]`
    * `[ Marked& Marked::`**`operator=`**`(T&& t) noexcept ]`
    * `const T& Marked::`**`get`**`() const noexcept`
    * `[ void Marked::`**`set`**`(const T& t) ]`
    * `[ void Marked::`**`set`**`(T&& t) noexcept ]`
    * `[explicit] Marked::`**`operator const T&`**`() const noexcept`
    * `[ explicit Marked::`**`operator bool`**`() const ]`
    * `[ T& Marked::`**`operator*`**`() noexcept ]`
    * `const T& Marked::`**`operator*`**`() const noexcept`
    * `[ T2* Marked::`**`operator->`**`() noexcept ]`
    * `const T* Marked::`**`operator->`**`() const noexcept`
    * `[ template <typename... Args> [result type] Marked:;`**`operator()`**`(Args&&... args) ]`
    * `[ template <typename... Args> [result type] Marked:;`**`operator()`**`(Args&&... args) const ]`
    * `[ template <typename I> [element type]& Marked::`**`operator[]`**`(I i) ]`
    * `[ template <typename I> const [element type]& Marked::`**`operator[]`**`(I i) const ]`
    * `static const std::string& Marked::`**`id_name`**`()`
* `[ bool` **`operator==`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ bool` **`operator!=`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ bool` **`operator<`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ bool` **`operator>`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ bool` **`operator<=`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ bool` **`operator>=`**`(const Marked& a, const Marked& b) noexcept ]`
* `[ Marked` **`operator+`**`(const Marked& m) ]`
* `[ Marked` **`operator-`**`(const Marked& m) ]`
* `[ Marked&` **`operator++`**`(Marked& m) ]`
* `[ Marked` **`operator++`**`(Marked& m, int) ]`
* `[ Marked&` **`operator--`**`(Marked& m) ]`
* `[ Marked` **`operator--`**`(Marked& m, int) ]`
* `[ Marked` **`operator+`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator-`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator*`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator/`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator%`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator&`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator|`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator^`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator<<`**`(const Marked& a, const Marked& b) ]`
* `[ Marked` **`operator>>`**`(const Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator+=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator-=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator*=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator/=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator%=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator&=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator|=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator^=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator<<=`**`(Marked& a, const Marked& b) ]`
* `[ Marked&` **`operator>>=`**`(Marked& a, const Marked& b) ]`
* `[ std::string` **`to_str`**`(const Marked& m) ]`
* `[ bool` **`from_str`**`(std::string_view s, Marked& m) ]`
* `[ Marked` **`from_str`**`(std::string_view s) ]`
* `[ void` **`to_json`**`(json& j, const Marked& m) ]`
* `[ void` **`from_json`**`(const json& j, Marked& m) ]`

Functions in brackets are optionally defined, as described below.

The `Marked` template is an approximation to a "strong type alias", a concept
that can't yet be fully emulated in C++. It associates a tag type (`ID`) with
an underlying value type (`T`). The `ID` type is used only for type checking
and plays no direct part in operations on the `Marked` type; normally `ID`
will be an empty class.

The third template argument, `Flags`, is a bitwise-or combination of flags
from the `Mark` enumeration. These control the semantics of the `Marked`
class, and have the following meanings:

Constant                     | Description
--------                     | -----------
`Mark::`**`none`**           | Empty bitmask
`Mark::`**`implicit_in`**    | Define an implicit conversion from `T` to `Marked`
`Mark::`**`implicit_out`**   | Define an implicit conversion from `Marked` to `T`
`Mark::`**`no_copy`**        | The `Marked` type is not copyable
`Mark::`**`no_move`**        | The `Marked` type is not copyable or movable (implies `no_copy`)
`Mark::`**`reset_on_move`**  | Moving resets the contained `T` to its default (implies `no_copy`)
`Mark::`**`arithmetic`**     | Arithmetic operators are defined for `Marked`
`Mark::`**`subscript`**      | A subscript operator is defined for `Marked`
`Mark::`**`string_tag`**     | The string format is tagged with the name of the ID type
`Mark::`**`json_tag`**       | The JSON format is tagged with the name of the ID type

The fourth template argument, `Check`, is an optional type that performs a
value check on any value of `T` supplied to a `Marked` object (through the
constructor, assignment operator, or the `set()` function). If `Check` is not
`void`, the input value will be passed through `Check()(t)`, and the return
value saved as the new value. `Check`'s function call operator can check the
validity of the input value (throwing an exception if it fails), modify the
value (returning the modified value), or both. If it only checks without
modifying, it may simply return the `const T&` it was passed.

If `Check` is used, some operations are disabled: operations that accept a
`T&&` and move from it are disabled (because a check failure could lead to the
value being thrown away), as are any functions that would give direct access
to a mutable reference to the embedded `T` value inside the `Marked` object
(such as the mutable versions of `operator*()` and `operator->()`).

The `id_name()` function returns the name of the `ID` type (demangled and with
all qualifications removed).

These conditions on `T` will be checked by `static_assert`:

* `T` must be a concrete type
* `T` must not be a reference type
* `T` must not be `const` or `volatile` qualified
* `T` must have consistent construction and assignment:
    `is_copy_constructible` and `is_copy_assignable` must match,
    as must `is_move_constructible` and `is_move_assignable`
* If `reset_on_move` is set, `T` must be default constructible
* If `Check` is not `void`, `T` must be copyable

Behaviour is undefined if any of the following are true (these are not checked
because a non-throwing requirement only applies at runtime; the operations
need not be `noexcept` qualified):

* `T`'s move constructor or move assignment operator throws
* The `reset_on_move` flag is set and `T`'s default constructor throws

In the table of conditionally defined functions below:

* The `Marked` type is **copyable** if `T` is copyable and none of `no_copy`, `no_move`, or `reset_on_move` are set
* The `Marked` type is **movable** if `T` is movable and `no_move` is not set
* The `Marked` type is **checked** if `Check` is not `void`

<table>
    <tr>
        <th>Function</th>
        <th>Conditionally defined</th>
    </tr>
    <tr>
        <td>Default constructor</td>
        <td>Defined if <code>T</code> is default constructible</td>
    </tr>
    <tr>
        <td>Copy constructor</td>
        <td>Defined if <code>Marked</code> is copyable</td>
    </tr>
    <tr>
        <td>Move constructor</td>
        <td>Defined if <code>Marked</code> is movable</td>
    </tr>
    <tr>
        <td>Constructor from <code>T</code></td>
        <td>Copy version is defined if <code>Marked</code> is copyable<br>
            Move version is defined if <code>Marked</code> is movable and the value is unchecked<br>
            Both versions are implicit if <code>implicit_in</code> is set, otherwise explicit</td>
    </tr>
    <tr>
        <td>Destructor</td>
        <td>Always defined</td>
    </tr>
    <tr>
        <td>Copy assignment operator</td>
        <td>Defined if <code>Marked</code> is copyable</td>
    </tr>
    <tr>
        <td>Move assignment operator</td>
        <td>Defined if <code>Marked</code> is movable</td>
    </tr>
    <tr>
        <td>Assignment from <code>T</code></td>
        <td>Copy version is defined if <code>Marked</code> is copyable and <code>implicit_in</code> is set<br>
            Move version is defined if <code>Marked</code> is movable, the value is unchecked, and <code>implicit_in</code> is set</td>
    </tr>
    <tr>
        <td><code>get()</code></td>
        <td>Always defined</td>
    </tr>
    <tr>
        <td><code>set()</code></td>
        <td>Copy version is defined if <code>T</code> is copyable<br>
            Move version is defined if <code>T</code> is movable andd the value is unchecked</td>
    </tr>
    <tr>
        <td>Conversion to <code>T</code></td>
        <td>Always defined<br>
            Implicit if <code>implicit_out</code> is set, otherwise explicit</td>
    </tr>
    <tr>
        <td>Conversion to <code>bool</code></td>
        <td>Defined if <code>T</code> is convertible to <code>bool</code><br>
            Always explicit</td>
    </tr>
    <tr>
        <td>Dereference operators</td>
        <td>Mutable versions are defined if the value is unchecked<br>
            Constant versions are always defined</td>
    </tr>
    <tr>
        <td>Function call operator</td>
        <td>Defined if the corresponding operator is defined for `T`</td>
    </tr>
    <tr>
        <td>Subscript operator</td>
        <td>Mutable version is defined if the <code>subscript</code> flag is set and the value is unchecked<br>
            Constant version is defined if the <code>subscript</code> flag is set</td>
    </tr>
    <tr>
        <td><code>id_name()</code></td>
        <td>Always defined</td>
    </tr>
    <tr>
        <td><code>== != &lt; &gt; &lt;= &gt;=</code></td>
        <td>Defined if the corresponding operator is defined for <code>T</code></td>
    </tr>
    <tr>
        <td>Unary <code>+ - ! ~</code><br>
            Prefix <code>++ --</code><br>
            Postfix <code>++ --</code><br>
            Binary <code>+ - * / % &amp; | ^ &lt;&lt; &gt;&gt;</code><br>
            Binary <code>+= -= *= /= %= &amp;= |= ^= &lt;&lt;= &gt;&gt;=</code></td>
        <td>Defined if the corresponding operator is defined for <code>T</code>
            and the <code>arithmetic</code> flag is set</td>
    </tr>
    <tr>
        <td><code>to_str()</code><br>
            <code>from_str()</code><br>
            <code>to_json()</code><br>
            <code>from_json()</code></td>
        <td>Defined if the corresponding function is defined for <code>T</code></td>
    </tr>
</table>

# Simple Table Layout #

By Ross Smith

* `#include "rs-core/table.hpp"`

## Contents ##

[TOC]

## Table class ##

**`Table`** is a utility class for simple table layout in fixed width text, a
task often useful in formatting the output from command line programs and
similar utilities.

Example:

    Table tab;
    tab << "Name" << "Number" << "Car" << '\n';
    tab.rule();
    tab << "Patrick McGoohan" << 6 << "Lotus 7" << '\n';
    tab << "James Bond" << 007 << "Aston Martin" << '\n';
    tab << "Douglas Adams" << 42 << "Ford Prefect" << '\n';
    tab << "Maxwell Smart" << 86 << "Sunbeam Tiger" << '\n';
    std::cout << tab;

Output:

    Name              Number  Car
    ================  ======  =============
    Patrick McGoohan  6       Lotus 7
    James Bond        7       Aston Martin
    Douglas Adams     42      Ford Prefect
    Maxwell Smart     86      Sunbeam Tiger

* `Table::`**`Table`**`()`
* `Table::`**`Table`**`(const Table& t)`
* `Table::`**`Table`**`(Table&& t) noexcept`
* `Table::`**`~Table`**`() noexcept`
* `Table& Table::`**`operator=`**`(const Table& t)`
* `Table& Table::`**`operator=`**`(Table&& t) noexcept`

Life cycle functions.

* `template <typename T> Table& Table::`**`operator<<`**`(const T& t)`

Adds a data cell to the table. The value is converted to a string using
`to_str()`. There is one special case: entering a single line feed character
(or a string containing only a line feed) ends the current row and starts a
new one.

* `void Table::`**`clear`**`() noexcept`

Clears all cell data and settings from the table (i.e. resets the table to its
default constructed state).

* `bool Table::`**`empty`**`() const noexcept`

True if the table contains no cell data.

* `void Table::`**`rule`**`(char c = [see below])`

Writes a horizontal rule across the table, by repeating the character to match
the width of each column. If no character is supplied, `'='` is used for the
first rule, `'-'` for subsequent ones.

* `void Table::`**`show_repeats`**`(bool flag = true) noexcept`

If this is set, any cell identical to the one immediately above it will be
replaced with `"''"`.

* `Ustring` **`to_str`**`(cons Table& tab)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Table& tab)`

Format the table as text. Two spaces will be inserted between columns; empty
cells will be written as `"--"`.

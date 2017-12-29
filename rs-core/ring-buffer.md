# Ring Buffer #

By Ross Smith

* `#include "rs-core/ring-buffer.hpp"`

## Contents ##

[TOC]

## Ring buffer class ##

* `template <typename T> class` **`RingBuffer`**

A ring buffer whose capacity is fixed at construction. The only requirement on
the value type `T` is that it must be at least either movable or copyable.

* `class RingBuffer::`**`iterator`**
* `using RingBuffer::`**`value_type`** `= T`

Member types. The iterator is a random access iterator that dereferences to a
`const T&`.

* `RingBuffer::`**`RingBuffer`**`()`
* `explicit RingBuffer::`**`RingBuffer`**`(size_t n)`
* `RingBuffer::`**`RingBuffer`**`(const RingBuffer& r)`
* `RingBuffer::`**`RingBuffer`**`(RingBuffer&& r) noexcept`
* `RingBuffer::`**`~RingBuffer`**`() noexcept`
* `RingBuffer& RingBuffer::`**`operator=`**`(const RingBuffer& r)`
* `RingBuffer& RingBuffer::`**`operator=`**`(RingBuffer&& r) noexcept`

Life cycle operations. A default constructed ring buffer has zero capacity.
The second constructor creates an empty ring buffer with a fixed capacity. A
moved-from ring buffer is empty, but differs from a default constructed one in
that it retains its capacity.

* `const T& RingBuffer::`**`operator[]`**`(size_t i) const noexcept`
* `RingBuffer::`**`iterator`**` RingBuffer::`**`begin`**`() const noexcept`
* `RingBuffer::`**`iterator`**` RingBuffer::`**`end`**`() const noexcept`
* `const T& RingBuffer::`**`front`**`() const noexcept`
* `const T& RingBuffer::`**`back`**`() const noexcept`

Element access functions. The `begin()` or `front()` end of the ring buffer is
the read end (the end that `pop()` extracts from); the `end()` or `back()` end
is the write end (the end that `push()` inserts into). Indexing starts at the
read end; `buffer[0]` is equivalent to `front()`. Behaviour is undefined if
the argument to `operator[]` is out of range, or if `front()` or `back()` are
called on an empty buffer.

* `size_t RingBuffer::`**`size`**`() const noexcept`

Returns the current number of elements in the buffer.

* `size_t RingBuffer::`**`capacity`**`() const noexcept`

Returns the maximum number of elements in the buffer.

* `bool RingBuffer::`**`empty`**`() const noexcept`
* `bool RingBuffer::`**`full`**`() const noexcept`

A buffer is empty if `size()=0`, full if `size()=capacity()`.

* `void RingBuffer::`**`push`**`(T&& t)`

Inserts a new element at the back of the buffer. This will delete the element
at the front if the buffer is already full. This does not invalidate any
iterators or element pointers, except ones that were pointing to the deleted
element, if any. Behaviour is undefined if this is called on a buffer with
zero capacity.

* `T RingBuffer::`**`pop`**`() noexcept`

Extracts and returns the element at the front of the buffer. Behaviour is
undefined if the buffer is empty. This invalidates only iterators or element
pointers that were pointing to the deleted element.

* `void RingBuffer::`**`clear`**`() noexcept`

Deletes all elements from the buffer. The capacity remains unchanged. All
iterators and element pointers are invalidated.

* `void RingBuffer::`**`swap`**`(RingBuffer& r) noexcept`
* `void` **`swap`**`(RingBuffer& r1, RingBuffer& r2) noexcept`

Swap two buffers. Iterators and element pointers remain valid, pointing to
their element's new position in the other buffer.

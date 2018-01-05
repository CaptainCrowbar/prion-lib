# Interprocess Communication #

By Ross Smith

* `#include "rs-core/ipc.hpp"`

## Contents ##

[TOC]

## Named mutex ##

* `class` **`NamedMutex`**
    * `explicit NamedMutex::`**`NamedMutex`**`(const Ustring& name)`
    * `NamedMutex::`**`~NamedMutex`**`() noexcept`
    * `Ustring NamedMutex::`**`name`**`() const`
    * `void NamedMutex::`**`lock`**`()`
    * `bool NamedMutex::`**`try_lock`**`()`
    * `template <typename R, typename P> bool NamedMutex::`**`try_lock_for`**`(std::chrono::duration<R, P> rel_time)`
    * `template <typename C, typename D> bool NamedMutex::`**`try_lock_until`**`(std::chrono::time_point<C, D> abs_time)`
    * `void NamedMutex::`**`unlock`**`() noexcept`

A named mutex that can be used for synchronization between processes. The
locking functions have the usual semantics required by the standard mutual
exclusion concept. The name may be any arbitrary string (a hash of the string
is used for the underlying system mutex name, to avoid system specific
restrictions on the name).

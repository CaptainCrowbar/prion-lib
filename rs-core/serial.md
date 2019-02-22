# JSON Serialization #

By Ross Smith

* `#include "rs-core/serial.hpp"`

This header imports the [`nlohmann/json`](https://github.com/nlohmann/json)
library and uses it for serialization of the types in my core library.

## Contents ##

[TOC]

## Imports ##

* `using nlohmann::`**`json`**

Imported for convenience.

## Serialization for standard library types ##

* `template <typename R, typename P> struct` **`adl_serializer`**`<std::chrono::duration<R, P>>`
* `template <typename C, typename D> struct` **`adl_serializer`**`<std::chrono::time_point<C, D>>`

These allow serialization of standard `<chrono>` types. Any duration can be
deserialized as a different duration type. Any time point can also be
deserialized as a different one, but any time point type associated with a
clock other than `system_clock` is converted between its native clock and
`system_clock` before serialization and after recovery (using
`convert_time_point()` from [`rs-core/time.hpp`](time.html)), which may
introduce rounding errors.

## Serialization for core library types ##

<!-- DEFN -->

Specializations of `to_json()` and `from_json()` are provided for:

* Containers
    * [`ArrayMap<K,T>`](array-map.html)
    * [`AutoVector<T>`](auto-array.html)
    * [`AutoDeque<T>`](auto-array.html)
    * [`Blob`](blob.html)
    * [`BoundedArray<T,N>`](bounded-array.html)
    * [`CompactArray<T,N>`](compact-array.html)
    * [`Grid<T,N>`](grid.html)
    * [`MirrorMap<K1,K2,C1,C2>`](mirror-map.html)
    * [`OrderedMap<K,T>`](ordered-map.html)
* Numeric types
    * [`Endian<T,B>`](common.html)
    * [`Int`](mp-integer.html)
    * [`Matrix<T,N,L>`](vector.html)
    * [`Nat`](mp-integer.html)
    * [`Quaternion<T>`](vector.html)
    * [`Rational<T>`](rational.html)
    * [`Vector<T,N>`](vector.html)
* Other classes
    * [`Optional<T>`](optional.html)
    * [`Uuid`](uuid.html)
    * [`Version`](common.html)
    * [`Xcolour`](terminal.html)

The serialization functions do not preserve template parameters such as the
size of a `Vector` or the comparison predicates in a `MirrorMap`; it is up to
the caller to ensure that the deserializing code uses a compatible type.

## Serialization helper functions ##

* `#define` **`RS_SERIALIZE_ENUM_AS_INTEGER`**`(EnumType)`
* `#define` **`RS_SERIALIZE_ENUM_AS_STRING`**`(EnumType)`

These macros define serialization functions (`to_json()` and `from_json()`)
for an enumeration type. The first macro stores the value as a number, and
will work with any `enum` or `enum class` type. The second stores the value as
a string containing the unqualified name of the value; this only works with
types defined using `RS_ENUM()` or `RS_ENUM_CLASS()`, and ensures that a value
with a given name will be restored to the same named value even if the mapping
from enumeration names to integer values has changed between serialization and
deserialization, provided the same name still exists. In all cases, the macro
must be invoked in the same namespace as the enumeration type.

* `template <typename T, typename FieldPtr, typename... Args> void` **`struct_to_json`**`(json& j, const T& t, const Ustring& name, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields)`
* `template <typename T, typename FieldPtr, typename... Args> void` **`json_to_struct`**`(const json& j, T& t, const Ustring& name, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields)`

These can be used to implement serialization functions (`to_json()` and
`from_json()`) for a user defined `struct` or `class` type. Each function
takes a JSON object, the user defined object to be serialized or deserialized,
a string identifying the type, and one or more pairs containing a data member
pointer and the name of that member.

The `struct_to_json()` function will add a `"_type"` field to the JSON object
if the type name supplied is not empty. For both functions, if the type name
is `"*"`, the unqualified name of the type will be used instead.

The `json_to_struct()` function will throw `std::invalid_argument` if the
input JSON value is not a JSON object (associative array). If the type name is
not empty, it will also check that the JSON object has a `"_type"` field that
matches the given name.

Example:

    struct Thing {
        std::string a, b;
        int c, d;
    };

    json j;
    Thing t = {"hello", "world", 86, 99};

    // Serialize a struct
    struct_to_json(j, t, "*",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    );

    //  j = {
    //      "_type": "Thing",
    //      "a": "hello",
    //      "b": "world",
    //      "c": 86,
    //      "d": 99
    //  }

    // Deserialize a struct
    json_to_struct(j, t, "*",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ));

## Persistent storage ##

* `class` **`PersistState`**
    * `explicit PersistState::`**`PersistState`**`(const Ustring& id)`
    * `template <typename... Args> explicit PersistState::`**`PersistState`**`(Args... id)`
    * `PersistState::`**`~PersistState`**`() noexcept`
    * `Ustring PersistState::`**`id`**`() const`
    * `Unicorn::Path PersistState::`**`file`**`() const`
    * `void PersistState::`**`load`**`()`
    * `void PersistState::`**`save`**`()`
    * `template <typename R, typename P> void PersistState::`**`autosave`**`(std::chrono::duration<R, P> t)`
    * `void PersistState::`**`autosave_off`**`()`
    * `void PersistState::`**`create`**`(const Ustring& key, const json& value)`
    * `bool PersistState::`**`read`**`(const Ustring& key, json& value)`
    * `void PersistState::`**`update`**`(const Ustring& key, const json& value)`
    * `void PersistState::`**`erase`**`(const Ustring& key)`

Common storage for state that needs to persist between runs of a program.
Normally only one of these will be constructed for a given program.

A program identifier is supplied to the constructor, either as a slash
delimited string or an explicit list of separate elements; typically this will
be in the form `"vendor/application"`. The identifier is used to create a file
in the user settings directory. To ensure file name portability, the following
restrictions are enforced on each slash delimited element of the identifier
(the constructor will throw `std::invalid_argument` if any of these rules are
broken):

* Must not be an empty string
* Must be valid UTF-8
* Must not be longer than 100 bytes (per element)
* Must not consist entirely of punctuation (i.e. must contain at least one alphanumeric or non-ASCII character)
* Must not contain any ASCII control characters (code points 0-31 and 127)
* Must not contain any of these characters: `" * / : < > ? [ \ ] |`
* Must not start or end with a space or dot

All operations on the `PersistState` object (apart from construction and
destruction) are fully thread safe; any member function can be called from
multiple threads without synchronization precautions.

The `load()` and `save()` functions load or save all of the persistent data to
the settings file in JSON form. A global named mutex is used to ensure that
loading and saving the same data from multiple processes running the same
program is safe; however, such processes will not see each other's saved data
unless they know to reload their state, and may overwrite each other's saves.
The `load()` and `save()` functions may throw exceptions from the
`nlohmann/json` library if  serialization or deserialization fails, or
`std::system_error` if an I/O error occurs. The constructor and destructor
call `load()` and `save()` respectively (failure in the final call to `save()`
is silently ignored).

Calling `autosave()` starts a thread that calls `save()` at regular intervals;
`autosave_off()` deactivates this. Calling `autosave()` with a zero or
negative interval is equivalent to calling `autosave_off()`.

The `create()`, `read()`, `update()`, and `delete()` functions manipulate the
saved data. The `create()` and `update()` functions will both create an entry
if it does not exist, but `create()` will use the supplied value only if one
is not already there, while `update()` will always overwrite any existing
value. All of these are used by the `Persist<T>` class and should not normally
be called explicitly.

* `template <typename T> class` **`Persist`**
    * `Persist::`**`Persist`**`() noexcept`
    * `Persist::`**`Persist`**`(PersistState& store, const Ustring& key, const T& init = {})`
    * `Persist::`**`~Persist`**`() noexcept`
    * `Persist::`**`Persist`**`(Persist&& p) noexcept`
    * `Persist& Persist::`**`operator=`**`(Persist&& p) noexcept`
    * `Persist& Persist::`**`operator=`**`(const T& t)`
    * `Persist::`**`operator T`**`() const`
    * `Ustring Persist::`**`key`**`() const`
    * `T Persist::`**`get`**`() const`
    * `void Persist::`**`set`**`(const T& t)`
    * `void Persist::`**`erase`**`()`

A variable whose state persists between runs of a program. The data type `T`
must be one for which the JSON serialization functions are available.

The key string supplied to the constructor can be any non-empty, valid UTF-8
string (the constructor will throw `std::invalid_argument` if the key is
invalid). The initial value supplied to the constructor will only be used if
no value is already stored for that key.

The value can be queried using `get()` or the conversion operator to `T`, and
updated using `set()` or the assignment operator from `T`. Values are stored
internally in serialized JSON form, so both query and update are nontrivial
operations; frequently used values should usually be copied into a separate
`T` variable.

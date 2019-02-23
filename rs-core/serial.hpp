#pragma once

#include "rs-core/array-map.hpp"
#include "rs-core/auto-array.hpp"
#include "rs-core/blob.hpp"
#include "rs-core/channel.hpp"
#include "rs-core/common.hpp"
#include "rs-core/grid.hpp"
#include "rs-core/ipc.hpp"
#include "rs-core/mirror-map.hpp"
#include "rs-core/mp-integer.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/ordered-map.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
#include "rs-core/terminal.hpp"
#include "rs-core/time.hpp"
#include "rs-core/uuid.hpp"
#include "rs-core/vector.hpp"
#include "unicorn/path.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <chrono>
#include <exception>
#include <memory>
#include <mutex>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <nlohmann/json.hpp>

namespace RS {

    // Import JSON class
    // https://github.com/nlohmann/json

    using nlohmann::json;

    // Serialization for core library numeric types

    template <typename T, ByteOrder B> void to_json(json& j, const Endian<T, B>& x) { j = x.get(); }
    template <typename T, ByteOrder B> void from_json(const json& j, Endian<T, B>& x) { x = j.get<T>(); }

    inline void to_json(json& j, const Int& x) { j = x.str(); }
    inline void from_json(const json& j, Int& x) { x = Int(j.get<Ustring>()); }

    template <typename T, size_t N, MatrixLayout L>
    void to_json(json& j, const Matrix<T, N, L>& x) {
        j = json::array();
        for (size_t r = 0; r < N; ++r)
            j[r] = x.row(r);
    }

    template <typename T, size_t N, MatrixLayout L>
    void from_json(const json& j, Matrix<T, N, L>& x) {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                x(r, c) = j[r][c].get<T>();
    }

    inline void to_json(json& j, const Nat& x) { j = x.str(); }
    inline void from_json(const json& j, Nat& x) { x = Nat(j.get<Ustring>()); }

    template <typename T>
    void to_json(json& j, const Quaternion<T>& x) {
        j = std::vector<T>(x.begin(), x.end());
    }

    template <typename T>
    void from_json(const json& j, Quaternion<T>& x) {
        auto v = j.get<std::vector<T>>();
        v.resize(4);
        std::copy(v.begin(), v.end(), x.begin());
    }

    template <typename T>
    void to_json(json& j, const Rational<T>& x) {
        j = json{x.num(), x.den()};
    }

    template <typename T>
    void from_json(const json& j, Rational<T>& x) {
        if (j.is_number()) {
            x = j.get<T>();
        } else {
            T num = j.at(0).get<T>();
            T den = j.at(1).get<T>();
            x = {num, den};
        }
    }

    template <typename T, size_t N>
    void to_json(json& j, const Vector<T, N>& x) {
        j = std::vector<T>(x.begin(), x.end());
    }

    template <typename T, size_t N>
    void from_json(const json& j, Vector<T, N>& x) {
        auto v = j.get<std::vector<T>>();
        v.resize(N);
        std::copy(v.begin(), v.end(), x.begin());
    }

    // Serialization for core library container types

    void to_json(json& j, const Blob& x);
    void from_json(const json& j, Blob& x);

    template <typename K, typename T>
    void to_json(json& j, const ArrayMap<K, T>& x) {
        j = json::array();
        for (auto& [k,t]: x)
            j.push_back({k, t});
    }

    template <typename K, typename T>
    void from_json(const json& j, ArrayMap<K, T>& x) {
        x.clear();
        std::pair<K, T> v;
        for (auto& row: j) {
            v.first = row[0].get<K>();
            v.second = row[1].get<T>();
            x.insert(v);
        }
    }

    template <typename T>
    void to_json(json& j, const AutoDeque<T>& x) {
        j["offset"] = x.min_index();
        auto& d = j["data"];
        d = json::array();
        for (auto& t: x)
            d.push_back(t);
    }

    template <typename T>
    void from_json(const json& j, AutoDeque<T>& x) {
        x.clear();
        size_t i = j["offset"].get<size_t>();
        for (auto& row: j["data"])
            x[i++] = row.get<T>();
    }

    template <typename T, size_t N>
    void to_json(json& j, const Grid<T, N>& x) {
        j["shape"] = x.shape();
        auto& d = j["data"];
        d = json::array();
        for (auto& t: x)
            d.push_back(t);
    }

    template <typename T, size_t N>
    void from_json(const json& j, Grid<T, N>& x) {
        using V = Vector<size_t, N>;
        V shape = j["shape"].get<V>();
        x.reset(shape);
        auto& d = j["data"];
        auto a = d.begin(), a_end = d.end();
        auto b = x.begin(), b_end = x.end();
        while (a != a_end && b != b_end)
            *b++ = a++->get<T>();
    }

    template <typename K1, typename K2, typename C1, typename C2>
    void to_json(json& j, const MirrorMap<K1, K2, C1, C2>& x) {
        j = json::array();
        for (auto& t: x.left())
            j.push_back({t.first, t.second});
    }

    template <typename K1, typename K2, typename C1, typename C2>
    void from_json(const json& j, MirrorMap<K1, K2, C1, C2>& x) {
        x.clear();
        std::pair<K1, K2> v;
        for (auto& row: j) {
            v.first = row[0].get<K1>();
            v.second = row[1].get<K2>();
            x.insert(v);
        }
    }

    template <typename K, typename T>
    void to_json(json& j, const OrderedMap<K, T>& x) {
        j = json::array();
        for (auto& t: x)
            j.push_back({t.first, t.second});
    }

    template <typename K, typename T>
    void from_json(const json& j, OrderedMap<K, T>& x) {
        x.clear();
        std::pair<K, T> v;
        for (auto& row: j) {
            v.first = row[0].get<K>();
            v.second = row[1].get<T>();
            x.insert(v);
        }
    }

    // Serialization for other core library types

    template <typename T> void to_json(json& j, const Optional<T>& x) { j = x ? json(*x) : json(); }
    template <typename T> void from_json(const json& j, Optional<T>& x) { x = j.is_null() ? Optional<T>() : j.get<T>(); }
    inline void to_json(json& j, const Uuid& x) { j = x.str(); }
    inline void from_json(const json& j, Uuid& x) { x = Uuid(j.get<Ustring>()); }
    inline void to_json(json& j, const Version& x) { j = x.str(); }
    inline void from_json(const json& j, Version& x) { x = Version(j.get<Ustring>()); }
    void to_json(json& j, const Xcolour& x);
    void from_json(const json& j, Xcolour& x);

    // Serialization helper functions

    #define RS_SERIALIZE_ENUM_AS_INTEGER(EnumType) \
        inline RS_ATTR_UNUSED void from_json(const json& j, EnumType& t) { \
            using U = std::underlying_type_t<EnumType>; \
            t = static_cast<EnumType>(j.get<U>()); \
        } \
        inline RS_ATTR_UNUSED void to_json(json& j, EnumType t) { \
            using U = std::underlying_type_t<EnumType>; \
            j = static_cast<U>(t); \
        }

    #define RS_SERIALIZE_ENUM_AS_STRING(EnumType) \
        inline RS_ATTR_UNUSED void from_json(const json& j, EnumType& t) { \
            auto s = j.get<Ustring>(); \
            if (! str_to_enum(s, t)) \
                throw std::invalid_argument("Invalid enumeration value: " + quote(s)); \
        } \
        inline RS_ATTR_UNUSED void to_json(json& j, EnumType t) { \
            j = unqualify(to_str(t)); \
        }

    namespace RS_Detail {

        template <typename T, typename FieldPtr, typename... Args>
        void fields_to_json(json& j, const T& t, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields) {
            j[field_name] = t.*field_ptr;
            if constexpr (sizeof...(Args) > 0)
                fields_to_json(j, t, more_fields...);
        }

        template <typename T, typename FieldPtr, typename... Args>
        void json_to_fields(const json& j, T& t, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields) {
            using FT = std::decay_t<decltype(t.*field_ptr)>;
            if (j.count(field_name))
                t.*field_ptr = j.at(field_name).get<FT>();
            if constexpr (sizeof...(Args) > 0)
                json_to_fields(j, t, more_fields...);
        }

    }

    template <typename T, typename FieldPtr, typename... Args>
    void struct_to_json(json& j, const T& t, const Ustring& name, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields) {
        j = json::object();
        if (name == "*")
            j["_type"] = unqualify(type_name<T>());
        else if (! name.empty())
            j["_type"] = name;
        RS_Detail::fields_to_json(j, t, field_ptr, field_name, more_fields...);
    }

    template <typename T, typename FieldPtr, typename... Args>
    void json_to_struct(const json& j, T& t, const Ustring& name, FieldPtr T::*field_ptr, const Ustring& field_name, Args... more_fields) {
        if (! j.is_object())
            throw std::invalid_argument("Invalid serialized object: Not a JSON object");
        if (! name.empty()) {
            if (! j.count("_type"))
                throw std::invalid_argument("Invalid serialized object: No type field");
            Ustring t_name;
            if (name == "*")
                t_name = unqualify(type_name<T>());
            else
                t_name = name;
            if (j["_type"] != t_name)
                throw std::invalid_argument("Invalid serialized " + t_name + ": Type is " + std::string(j["_type"]));
        }
        RS_Detail::json_to_fields(j, t, field_ptr, field_name, more_fields...);
    }

    // Persistent storage

    class PersistState {
    public:
        RS_NO_COPY_MOVE(PersistState);
        explicit PersistState(const Ustring& id);
        template <typename... Args> explicit PersistState(Args... id): PersistState(join(Strings{id...}, "/")) {}
        ~PersistState() noexcept;
        Unicorn::Path file() const { return archive_name(); }
        Ustring id() const { return state_id; }
        void load();
        void save() { save_state(true); }
        template <typename R, typename P> void autosave(std::chrono::duration<R, P> t);
        void autosave_off();
        void create(const Ustring& key, const json& value);
        bool read(const Ustring& key, json& value);
        void update(const Ustring& key, const json& value);
        void erase(const Ustring& key);
    private:
        std::unique_ptr<TimerChannel> autosave_channel;
        Thread autosave_thread;
        std::unique_ptr<NamedMutex> global_mutex;
        std::mutex local_mutex;
        Ustring state_id;
        json state_table;
        bool change_flag = false;
        Unicorn::Path archive_name(const Ustring& tag = {}) const;
        void autosave_loop();
        void clear_autosave();
        void save_state(bool always);
    };

        template <typename R, typename P>
        void PersistState::autosave(std::chrono::duration<R, P> t) {
            using namespace std::chrono;
            auto local_lock = make_lock(local_mutex);
            clear_autosave();
            if (t > duration<R, P>()) {
                autosave_channel = std::make_unique<TimerChannel>(t);
                autosave_thread = Thread([this] { autosave_loop(); });
            }
        }

    // Persistent data wrapper

    template <typename T>
    class Persist {
    public:
        RS_MOVE_ONLY(Persist);
        Persist() = default;
        Persist(PersistState& store, const Ustring& key, const T& init = {});
        ~Persist() = default;
        Persist& operator=(const T& t) { set(t); return *this; }
        operator T() const { return get(); }
        Ustring key() const { return db_key; }
        T get() const;
        void set(const T& t);
        void erase() { db_ptr->erase(db_key); }
    private:
        PersistState* db_ptr = nullptr;
        Ustring db_key;
    };

        template <typename T>
        Persist<T>::Persist(PersistState& store, const Ustring& key, const T& init):
        db_ptr(&store), db_key(key) {
            if (key.empty() || ! Unicorn::valid_string(key))
                throw std::invalid_argument("Invalid persistent storage key: " + quote(key));
            db_ptr->create(db_key, init);
        }

        template <typename T>
        T Persist<T>::get() const {
            if (! db_ptr)
                return {};
            json j;
            if (! db_ptr->read(db_key, j))
                return {};
            try { return j.get<T>(); }
                catch (const std::bad_alloc&) { throw; }
                catch (const std::exception&) { return {}; }
        }

        template <typename T>
        void Persist<T>::set(const T& t) {
            if (db_ptr)
                db_ptr->update(db_key, json(t));
        }

}

namespace nlohmann {

    // Serialization for std::chrono types

    template <typename R, typename P>
    struct adl_serializer<std::chrono::duration<R, P>> {
        using duration_type = std::chrono::duration<R, P>;
        static void to_json(json& j, const duration_type& x) {
            j = json{x.count(), int64_t(P::num), int64_t(P::den)};
        }
        static void from_json(const json& j, duration_type& x) {
            using namespace std::chrono;
            static constexpr double xnum = double(P::num);
            static constexpr double xden = double(P::den);
            double count = j.at(0).get<double>();
            double jnum = j.at(1).get<double>();
            double jden = j.at(2).get<double>();
            double num = jnum * xden, den = jden * xnum;
            if (num >= den)
                count *= num / den;
            else
                count /= den / num;
            x = duration_type(R(count));
        }
    };

    template <typename D>
    struct adl_serializer<std::chrono::time_point<std::chrono::system_clock, D>> {
        using time_point = std::chrono::time_point<std::chrono::system_clock, D>;
        static void to_json(json& j, const time_point& x) {
            using namespace std::chrono;
            auto sys = time_point_cast<system_clock::duration>(x);
            j = sys.time_since_epoch();
        }
        static void from_json(const json& j, time_point& x) {
            using namespace std::chrono;
            system_clock::duration dur = j;
            system_clock::time_point sys(dur);
            x = time_point_cast<D>(sys);
        }
    };

    template <typename C, typename D>
    struct adl_serializer<std::chrono::time_point<C, D>> {
        using time_point = std::chrono::time_point<C, D>;
        static void to_json(json& j, const time_point& x) {
            using namespace std::chrono;
            system_clock::time_point sys;
            RS::convert_time_point(x, sys);
            j = sys;
        }
        static void from_json(const json& j, time_point& x) {
            using namespace std::chrono;
            system_clock::time_point sys = j;
            RS::convert_time_point(sys, x);
        }
    };

}

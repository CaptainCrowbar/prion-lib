#pragma once

#include "rs-core/blob.hpp"
#include "rs-core/channel.hpp"
#include "rs-core/common.hpp"
#include "rs-core/ipc.hpp"
#include "rs-core/mp-integer.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/rational.hpp"
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
#include <vector>
#include <nlohmann/json.hpp>

namespace RS {

    // Import JSON class
    // https://github.com/nlohmann/json

    using nlohmann::json;

    // Serialization for core library types

    void to_json(json& j, const Blob& x);
    void from_json(const json& j, Blob& x);

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

    template <typename T> void to_json(json& j, const Optional<T>& x) { j = x ? json(*x) : json(); }
    template <typename T> void from_json(const json& j, Optional<T>& x) { x = j.is_null() ? Optional<T>() : j.get<T>(); }

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

    inline void to_json(json& j, const Uuid& x) { j = x.str(); }
    inline void from_json(const json& j, Uuid& x) { x = Uuid(j.get<Ustring>()); }

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

    inline void to_json(json& j, const Version& x) { j = x.str(); }
    inline void from_json(const json& j, Version& x) { x = Version(j.get<Ustring>()); }

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

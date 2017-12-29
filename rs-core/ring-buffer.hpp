#pragma once

#include "rs-core/common.hpp"
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    template <typename T>
    class RingBuffer {
    public:
        class iterator: public RandomAccessIterator<iterator, const T> {
        public:
            iterator() = default;
            const T& operator*() const noexcept { return (*buf)[ofs]; }
            iterator& operator+=(ptrdiff_t rhs) noexcept { ofs += rhs; return *this; }
            ptrdiff_t operator-(const iterator& rhs) const noexcept { return ptrdiff_t(ofs) - ptrdiff_t(rhs.ofs); }
        private:
            friend class RingBuffer;
            const RingBuffer* buf;
            size_t ofs;
            iterator(const RingBuffer* r, size_t i) noexcept: buf(r), ofs(i) {}
        };
        using value_type = T;
        RingBuffer() = default;
        explicit RingBuffer(size_t n): vec(n), pos(0), len(0) {}
        RingBuffer(const RingBuffer& r);
        RingBuffer(RingBuffer&& r) noexcept;
        ~RingBuffer() noexcept { clear(); }
        RingBuffer& operator=(const RingBuffer& r);
        RingBuffer& operator=(RingBuffer&& r) noexcept;
        const T& operator[](size_t i) const noexcept { return ptr()[index(i)]; }
        iterator begin() const noexcept { return {this, 0}; }
        iterator end() const noexcept { return {this, len}; }
        const T& front() const noexcept { return ptr()[pos]; }
        const T& back() const noexcept { return ptr()[index(len - 1)]; }
        size_t size() const noexcept { return len; }
        size_t capacity() const noexcept { return vec.size(); }
        bool empty() const noexcept { return len == 0; }
        bool full() const noexcept { return len == vec.size(); }
        void push(T&& t);
        T pop() noexcept;
        void clear() noexcept;
        void swap(RingBuffer& r) noexcept;
    private:
        using cell = typename std::aligned_union<1, T>::type;
        static_assert(sizeof(cell) == sizeof(T));
        std::vector<cell> vec;
        size_t pos = 0;
        size_t len = 0;
        size_t index(ptrdiff_t i) const noexcept { return rem((ptrdiff_t(pos) + i), ptrdiff_t(capacity())); }
        T* ptr() noexcept { return reinterpret_cast<T*>(vec.data()); }
        const T* ptr() const noexcept { return reinterpret_cast<const T*>(vec.data()); }
    };

    template <typename T>
    RingBuffer<T>::RingBuffer(const RingBuffer& r):
    vec(r.capacity()), pos(r.pos), len(r.len) {
        if (pos + len > capacity()) {
            size_t extra = pos + len - capacity();
            for (size_t i = pos; i < capacity(); ++i)
                new (vec.data() + i) T(r.ptr()[i]);
            for (size_t i = 0; i < extra; ++i)
                new (vec.data() + i) T(r.ptr()[i]);
        } else if (len) {
            for (size_t i = pos; i < pos + len; ++i)
                new (vec.data() + i) T(r.ptr()[i]);
        }
    }

    template <typename T>
    RingBuffer<T>::RingBuffer(RingBuffer&& r) noexcept {
        if (pos + len > capacity()) {
            size_t extra = pos + len - capacity();
            for (size_t i = pos; i < capacity(); ++i)
                new (vec.data() + i) T(std::move(r.ptr()[i]));
            for (size_t i = 0; i < extra; ++i)
                new (vec.data() + i) T(std::move(r.ptr()[i]));
        } else if (len) {
            for (size_t i = pos; i < pos + len; ++i)
                new (vec.data() + i) T(std::move(r.ptr()[i]));
        }
        r.clear();
    }

    template <typename T>
    RingBuffer<T>& RingBuffer<T>::operator=(const RingBuffer& r) {
        RingBuffer r2(r);
        swap(r2);
        return *this;
    }

    template <typename T>
    RingBuffer<T>& RingBuffer<T>::operator=(RingBuffer&& r) noexcept {
        swap(r);
        r.clear();
        return *this;
    }

    template <typename T>
    void RingBuffer<T>::push(T&& t) {
        if (full()) {
            ptr()[pos] = std::forward<T>(t);
            pos = index(1);
        } else {
            new (vec.data() + index(len)) T(std::forward<T>(t));
            ++len;
        }
    }

    template <typename T>
    T RingBuffer<T>::pop() noexcept {
        T t = std::move(ptr()[pos]);
        ptr()[pos].~T();
        if (len == 1) {
            pos = len = 0;
        } else {
            pos = index(1);
            --len;
        }
        return t;
    }

    template <typename T>
    void RingBuffer<T>::clear() noexcept {
        if (pos + len > capacity()) {
            size_t extra = pos + len - capacity();
            for (size_t i = pos; i < capacity(); ++i)
                ptr()[i].~T();
            for (size_t i = 0; i < extra; ++i)
                ptr()[i].~T();
        } else if (len) {
            for (size_t i = pos; i < pos + len; ++i)
                ptr()[i].~T();
        }
        pos = len = 0;
    }

    template <typename T>
    void RingBuffer<T>::swap(RingBuffer& r) noexcept {
        vec.swap(r.vec);
        std::swap(pos, r.pos);
        std::swap(len, r.len);
    }

    template <typename T>
    void swap(RingBuffer<T>& r1, RingBuffer<T>& r2) noexcept {
        r1.swap(r2);
    }

}

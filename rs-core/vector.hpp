#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <type_traits>

namespace RS {

    // Vector

    template <typename T, size_t N>
    class Vector:
    public EqualityComparable<Vector<T, N>> {
    public:
        using value_type = T;
        static constexpr size_t dim = N;
        Vector() noexcept { std::fill(begin(), end(), T(0)); }
        template <typename T2> explicit Vector(T2 t) noexcept { std::fill(begin(), end(), implicit_cast<T>(t)); }
        template <typename... Args> Vector(Args... args) noexcept: arr{{implicit_cast<T>(args)...}} {}
        template <typename T2> explicit Vector(const T2* ptr) noexcept { std::transform(ptr, ptr + N, arr.data(), implicit_cast<T, T2>); }
        template <typename T2, size_t N2> Vector(const Vector<T2, N2>& v) noexcept;
        template <typename T2, size_t N2> Vector& operator=(const Vector<T2, N2>& v) noexcept;
        T& operator[](size_t i) noexcept { return arr[i]; }
        const T& operator[](size_t i) const noexcept { return arr[i]; }
        template <size_t N2> Vector<T, N2 - 1> operator[](const char (&str)[N2]) const;
        Vector operator+() const noexcept { return *this; }
        Vector operator-() const noexcept;
        template <typename T2> Vector& operator+=(const Vector<T2, N>& rhs) noexcept;
        template <typename T2> Vector& operator-=(const Vector<T2, N>& rhs) noexcept;
        template <typename T2> Vector& operator*=(const Vector<T2, N>& rhs) noexcept;
        template <typename T2> Vector& operator/=(const Vector<T2, N>& rhs) noexcept;
        template <typename T2> Vector& operator*=(T2 rhs) noexcept;
        template <typename T2> Vector& operator/=(T2 rhs) noexcept;
        T& at(size_t i) { range_check(i); return arr[i]; }
        const T& at(size_t i) const { range_check(i); return arr[i]; }
        T& x() noexcept { return arr[0]; }
        const T& x() const noexcept { return arr[0]; }
        T& y() noexcept { return arr[1]; }
        const T& y() const noexcept { return arr[1]; }
        T& z() noexcept { return arr[2]; }
        const T& z() const noexcept { return arr[2]; }
        T& w() noexcept { return arr[3]; }
        const T& w() const noexcept { return arr[3]; }
        T* begin() noexcept { return arr.data(); }
        const T* begin() const noexcept { return arr.data(); }
        T* end() noexcept { return begin() + N; }
        const T* end() const noexcept { return begin() + N; }
        T angle(const Vector& v) const noexcept;
        Vector dir() const noexcept { return *this == Vector() ? Vector() : *this / r(); }
        bool is_null() const noexcept { return *this == Vector(); }
        Vector project(const Vector& v) const noexcept { return v == Vector() ? Vector() : v * ((*this % v) / (v % v)); }
        Vector reject(const Vector& v) const noexcept { return *this - project(v); }
        T r() const noexcept { return std::sqrt(r2()); }
        T r2() const noexcept { return std::inner_product(begin(), end(), begin(), T(0)); }
        constexpr size_t size() const noexcept { return N; }
        static Vector unit(size_t i) noexcept;
        static Vector zero() noexcept { return {}; }
    private:
        static_assert(N > 0);
        std::array<T, N> arr;
        static void range_check(size_t i) { if (i >= N) throw std::out_of_range("Vector index is out of range"); }
    };

    using Int1 = Vector<int, 1>;
    using Int2 = Vector<int, 2>;
    using Int3 = Vector<int, 3>;
    using Int4 = Vector<int, 4>;
    using Int8_1 = Vector<int8_t, 1>;       using Uint8_1 = Vector<uint8_t, 1>;
    using Int8_2 = Vector<int8_t, 2>;       using Uint8_2 = Vector<uint8_t, 2>;
    using Int8_3 = Vector<int8_t, 3>;       using Uint8_3 = Vector<uint8_t, 3>;
    using Int8_4 = Vector<int8_t, 4>;       using Uint8_4 = Vector<uint8_t, 4>;
    using Int16_1 = Vector<int16_t, 1>;     using Uint16_1 = Vector<uint16_t, 1>;
    using Int16_2 = Vector<int16_t, 2>;     using Uint16_2 = Vector<uint16_t, 2>;
    using Int16_3 = Vector<int16_t, 3>;     using Uint16_3 = Vector<uint16_t, 3>;
    using Int16_4 = Vector<int16_t, 4>;     using Uint16_4 = Vector<uint16_t, 4>;
    using Int32_1 = Vector<int32_t, 1>;     using Uint32_1 = Vector<uint32_t, 1>;
    using Int32_2 = Vector<int32_t, 2>;     using Uint32_2 = Vector<uint32_t, 2>;
    using Int32_3 = Vector<int32_t, 3>;     using Uint32_3 = Vector<uint32_t, 3>;
    using Int32_4 = Vector<int32_t, 4>;     using Uint32_4 = Vector<uint32_t, 4>;
    using Int64_1 = Vector<int64_t, 1>;     using Uint64_1 = Vector<uint64_t, 1>;
    using Int64_2 = Vector<int64_t, 2>;     using Uint64_2 = Vector<uint64_t, 2>;
    using Int64_3 = Vector<int64_t, 3>;     using Uint64_3 = Vector<uint64_t, 3>;
    using Int64_4 = Vector<int64_t, 4>;     using Uint64_4 = Vector<uint64_t, 4>;
    using Ptrdiff1 = Vector<ptrdiff_t, 1>;  using Size1 = Vector<size_t, 1>;
    using Ptrdiff2 = Vector<ptrdiff_t, 2>;  using Size2 = Vector<size_t, 2>;
    using Ptrdiff3 = Vector<ptrdiff_t, 3>;  using Size3 = Vector<size_t, 3>;
    using Ptrdiff4 = Vector<ptrdiff_t, 4>;  using Size4 = Vector<size_t, 4>;
    using Float1 = Vector<float, 1>;        using Double1 = Vector<double, 1>;  using Ldouble1 = Vector<long double, 1>;
    using Float2 = Vector<float, 2>;        using Double2 = Vector<double, 2>;  using Ldouble2 = Vector<long double, 2>;
    using Float3 = Vector<float, 3>;        using Double3 = Vector<double, 3>;  using Ldouble3 = Vector<long double, 3>;
    using Float4 = Vector<float, 4>;        using Double4 = Vector<double, 4>;  using Ldouble4 = Vector<long double, 4>;

    template <typename T, size_t N>
    template <typename T2, size_t N2>
    Vector<T, N>::Vector(const Vector<T2, N2>& v) noexcept {
        static_assert(N2 == N);
        std::transform(v.begin(), v.end(), begin(), implicit_cast<T, T2>);
    }

    template <typename T, size_t N>
    template <typename T2, size_t N2>
    Vector<T, N>& Vector<T, N>::operator=(const Vector<T2, N2>& v) noexcept {
        static_assert(N2 == N);
        std::copy(v.begin(), v.end(), begin());
        return *this;
    }

    template <typename T, size_t N>
    template <size_t N2>
    Vector<T, N2 - 1> Vector<T, N>::operator[](const char (&str)[N2]) const {
        Vector<T, N2 - 1> swiz;
        for (size_t i = 0; i < N2 - 1; ++i)
            swiz[i] = arr[str[i] == 'w' ? 3 : str[i] - 'x'];
        return swiz;
    }

    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator-() const noexcept {
        Vector v;
        std::transform(begin(), end(), v.begin(), [] (T t) { return - t; });
        return v;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator+=(const Vector<T2, N>& rhs) noexcept {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] += rhs[i];
        return *this;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator-=(const Vector<T2, N>& rhs) noexcept {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] -= rhs[i];
        return *this;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator*=(const Vector<T2, N>& rhs) noexcept {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] *= rhs[i];
        return *this;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator/=(const Vector<T2, N>& rhs) noexcept {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] /= rhs[i];
        return *this;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator*=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t *= rhs; });
        return *this;
    }

    template <typename T, size_t N>
    template <typename T2>
    Vector<T, N>& Vector<T, N>::operator/=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t /= rhs; });
        return *this;
    }

    template <typename T, size_t N>
    T Vector<T, N>::angle(const Vector& v) const noexcept {
        if (is_null() || v.is_null())
            return T(0);
        else
            return std::acos(*this % v / std::sqrt(r2() * v.r2()));
    }

    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::unit(size_t i) noexcept {
        Vector v;
        v[i] = T(1);
        return v;
    }


    template <typename T1, typename T2>
    Vector<T1, 3> operator^(const Vector<T1, 3>& lhs, const Vector<T2, 3>& rhs) noexcept {
        return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
                lhs[2] * rhs[0] - lhs[0] * rhs[2],
                lhs[0] * rhs[1] - lhs[1] * rhs[0]};
    }

    template <typename T1, typename T2, size_t N>
    T1 operator%(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept {
        return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), T1(0));
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator+(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept {
        auto v = lhs;
        v += rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator-(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept {
        auto v = lhs;
        v -= rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator*(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept {
        auto v = lhs;
        v *= rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator/(const Vector<T1, N>& lhs, const Vector<T2, N>& rhs) noexcept {
        auto v = lhs;
        v /= rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator*(const Vector<T1, N>& lhs, T2 rhs) noexcept {
        auto v = lhs;
        v *= rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T2, N> operator*(T1 lhs, const Vector<T2, N>& rhs) noexcept {
        Vector<T2, N> v(lhs);
        v *= rhs;
        return v;
    }

    template <typename T1, typename T2, size_t N>
    Vector<T1, N> operator/(const Vector<T1, N>& lhs, T2 rhs) noexcept {
        auto v = lhs;
        v /= rhs;
        return v;
    }

    template <typename T, size_t N>
    bool operator==(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, size_t N>
    bool operator<(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T, size_t N>
    bool operator>(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
        return rhs < lhs;
    }

    template <typename T, size_t N>
    bool operator<=(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
        return ! (rhs < lhs);
    }

    template <typename T, size_t N>
    bool operator>=(const Vector<T, N>& lhs, const Vector<T, N>& rhs) noexcept {
        return ! (lhs < rhs);
    }

    template <typename T, size_t N>
    std::ostream& operator<<(std::ostream& out, const Vector<T, N>& v) {
        return out << to_str(v);
    }

    // Matrix

    enum class MatrixLayout {
        column = 'C',
        row = 'R',
    };

    namespace RS_Detail {

        template <typename T, size_t N, MatrixLayout L>
        struct LayoutSpecific;

        template <typename T, size_t N>
        struct LayoutSpecific<T, N, MatrixLayout::column> {
            static T& get_ref(T* ptr, size_t r, size_t c) noexcept { return ptr[r + N * c]; }
            static const T& get_ref(const T* ptr, size_t r, size_t c) noexcept { return ptr[r + N * c]; }
            static Vector<T, N> get_column(const T* ptr, size_t c) noexcept {
                return Vector<T, N>(ptr + N * c);
            }
            static Vector<T, N> get_row(const T* ptr, size_t r) noexcept {
                Vector<T, N> v;
                for (size_t i = 0, j = r; i < N; ++i, j += N)
                    v[i] = ptr[j];
                return v;
            }
            static void swap_columns(T* ptr, size_t c1, size_t c2) noexcept {
                if (c1 != c2)
                    std::swap_ranges(ptr + N * c1, ptr + N * (c1 + 1), ptr + N * c2);
            }
            static void swap_rows(T* ptr, size_t r1, size_t r2) noexcept {
                if (r1 != r2)
                    for (; r1 < N * N; r1 += N, r2 += N)
                        std::swap(ptr[r1], ptr[r2]);
            }
        };

        template <typename T, size_t N>
        struct LayoutSpecific<T, N, MatrixLayout::row> {
            static T& get_ref(T* ptr, size_t r, size_t c) noexcept { return ptr[N * r + c]; }
            static const T& get_ref(const T* ptr, size_t r, size_t c) noexcept { return ptr[N * r + c]; }
            static Vector<T, N> get_column(const T* ptr, size_t c) noexcept {
                Vector<T, N> v;
                for (size_t i = 0, j = c; i < N; ++i, j += N)
                    v[i] = ptr[j];
                return v;
            }
            static Vector<T, N> get_row(const T* ptr, size_t r) noexcept {
                return Vector<T, N>(ptr + N * r);
            }
            static void swap_columns(T* ptr, size_t c1, size_t c2) noexcept {
                if (c1 != c2)
                    for (; c1 < N * N; c1 += N, c2 += N)
                        std::swap(ptr[c1], ptr[c2]);
            }
            static void swap_rows(T* ptr, size_t r1, size_t r2) noexcept {
                if (r1 != r2)
                    std::swap_ranges(ptr + N * r1, ptr + N * (r1 + 1), ptr + N * r2);
            }
        };

        template <typename M, size_t N = M::dim>
        struct Determinant;

        template <typename M>
        struct Determinant<M, 1> {
            auto operator()(const M& m) const noexcept {
                return m(0,0);
            }
        };

        template <typename M>
        struct Determinant<M, 2> {
            auto operator()(const M& m) const noexcept {
                return m(0,0) * m(1,1) - m(0,1) * m(1,0);
            }
        };

        template <typename M>
        struct Determinant<M, 3> {
            auto operator()(const M& m) const noexcept {
                return m(0,0) * m(1,1) * m(2,2)
                     + m(0,1) * m(1,2) * m(2,0)
                     + m(0,2) * m(1,0) * m(2,1)
                     - m(0,2) * m(1,1) * m(2,0)
                     - m(0,1) * m(1,0) * m(2,2)
                     - m(0,0) * m(1,2) * m(2,1);
            }
        };

        template <typename M>
        struct Determinant<M, 4> {
            auto operator()(const M& m) const noexcept {
                return m(0,0) * m(1,1) * m(2,2) * m(3,3)
                     + m(0,0) * m(2,1) * m(3,2) * m(1,3)
                     + m(0,0) * m(3,1) * m(1,2) * m(2,3)
                     + m(1,0) * m(0,1) * m(3,2) * m(2,3)
                     + m(1,0) * m(2,1) * m(0,2) * m(3,3)
                     + m(1,0) * m(3,1) * m(2,2) * m(0,3)
                     + m(2,0) * m(0,1) * m(1,2) * m(3,3)
                     + m(2,0) * m(1,1) * m(3,2) * m(0,3)
                     + m(2,0) * m(3,1) * m(0,2) * m(1,3)
                     + m(3,0) * m(0,1) * m(2,2) * m(1,3)
                     + m(3,0) * m(1,1) * m(0,2) * m(2,3)
                     + m(3,0) * m(2,1) * m(1,2) * m(0,3)
                     - m(0,0) * m(1,1) * m(3,2) * m(2,3)
                     - m(0,0) * m(2,1) * m(1,2) * m(3,3)
                     - m(0,0) * m(3,1) * m(2,2) * m(1,3)
                     - m(1,0) * m(0,1) * m(2,2) * m(3,3)
                     - m(1,0) * m(2,1) * m(3,2) * m(0,3)
                     - m(1,0) * m(3,1) * m(0,2) * m(2,3)
                     - m(2,0) * m(0,1) * m(3,2) * m(1,3)
                     - m(2,0) * m(1,1) * m(0,2) * m(3,3)
                     - m(2,0) * m(3,1) * m(1,2) * m(0,3)
                     - m(3,0) * m(0,1) * m(1,2) * m(2,3)
                     - m(3,0) * m(1,1) * m(2,2) * m(0,3)
                     - m(3,0) * m(2,1) * m(0,2) * m(1,3);
            }
        };

        template <typename M, size_t N = M::dim>
        struct Inverse;

        template <typename M>
        struct Inverse<M, 1> {
            using T = typename M::value_type;
            M operator()(const M& m) const noexcept {
                return {T(1) / m(0,0)};
            }
        };

        template <typename M>
        struct Inverse<M, 2> {
            using T = typename M::value_type;
            M operator()(const M& m) const noexcept {
                M co;
                co(0,0) = m(1,1);
                co(0,1) = - m(0,1);
                co(1,0) = - m(1,0);
                co(1,1) = m(0,0);
                return co / m.det();
            }
        };

        template <typename M>
        struct Inverse<M, 3> {
            using T = typename M::value_type;
            M operator()(const M& m) const noexcept {
                M co;
                co(0,0) = m(1,1) * m(2,2) - m(1,2) * m(2,1);
                co(0,1) = m(0,2) * m(2,1) - m(0,1) * m(2,2);
                co(0,2) = m(0,1) * m(1,2) - m(0,2) * m(1,1);
                co(1,0) = m(1,2) * m(2,0) - m(1,0) * m(2,2);
                co(1,1) = m(0,0) * m(2,2) - m(0,2) * m(2,0);
                co(1,2) = m(0,2) * m(1,0) - m(0,0) * m(1,2);
                co(2,0) = m(1,0) * m(2,1) - m(1,1) * m(2,0);
                co(2,1) = m(0,1) * m(2,0) - m(0,0) * m(2,1);
                co(2,2) = m(0,0) * m(1,1) - m(0,1) * m(1,0);
                return co / m.det();
            }
        };

        template <typename M>
        struct Inverse<M, 4> {
            using T = typename M::value_type;
            M operator()(const M& m) const noexcept {
                // http://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
                auto s0 = m(0,0) * m(1,1) - m(1,0) * m(0,1);
                auto s1 = m(0,0) * m(1,2) - m(1,0) * m(0,2);
                auto s2 = m(0,0) * m(1,3) - m(1,0) * m(0,3);
                auto s3 = m(0,1) * m(1,2) - m(1,1) * m(0,2);
                auto s4 = m(0,1) * m(1,3) - m(1,1) * m(0,3);
                auto s5 = m(0,2) * m(1,3) - m(1,2) * m(0,3);
                auto c5 = m(2,2) * m(3,3) - m(3,2) * m(2,3);
                auto c4 = m(2,1) * m(3,3) - m(3,1) * m(2,3);
                auto c3 = m(2,1) * m(3,2) - m(3,1) * m(2,2);
                auto c2 = m(2,0) * m(3,3) - m(3,0) * m(2,3);
                auto c1 = m(2,0) * m(3,2) - m(3,0) * m(2,2);
                auto c0 = m(2,0) * m(3,1) - m(3,0) * m(2,1);
                auto invdet = T(1) / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
                M result;
                result(0,0) = (m(1,1) * c5 - m(1,2) * c4 + m(1,3) * c3) * invdet;
                result(0,1) = (-m(0,1) * c5 + m(0,2) * c4 - m(0,3) * c3) * invdet;
                result(0,2) = (m(3,1) * s5 - m(3,2) * s4 + m(3,3) * s3) * invdet;
                result(0,3) = (-m(2,1) * s5 + m(2,2) * s4 - m(2,3) * s3) * invdet;
                result(1,0) = (-m(1,0) * c5 + m(1,2) * c2 - m(1,3) * c1) * invdet;
                result(1,1) = (m(0,0) * c5 - m(0,2) * c2 + m(0,3) * c1) * invdet;
                result(1,2) = (-m(3,0) * s5 + m(3,2) * s2 - m(3,3) * s1) * invdet;
                result(1,3) = (m(2,0) * s5 - m(2,2) * s2 + m(2,3) * s1) * invdet;
                result(2,0) = (m(1,0) * c4 - m(1,1) * c2 + m(1,3) * c0) * invdet;
                result(2,1) = (-m(0,0) * c4 + m(0,1) * c2 - m(0,3) * c0) * invdet;
                result(2,2) = (m(3,0) * s4 - m(3,1) * s2 + m(3,3) * s0) * invdet;
                result(2,3) = (-m(2,0) * s4 + m(2,1) * s2 - m(2,3) * s0) * invdet;
                result(3,0) = (-m(1,0) * c3 + m(1,1) * c1 - m(1,2) * c0) * invdet;
                result(3,1) = (m(0,0) * c3 - m(0,1) * c1 + m(0,2) * c0) * invdet;
                result(3,2) = (-m(3,0) * s3 + m(3,1) * s1 - m(3,2) * s0) * invdet;
                result(3,3) = (m(2,0) * s3 - m(2,1) * s1 + m(2,2) * s0) * invdet;
                return result;
            }
        };

    }

    template <typename T, size_t N, MatrixLayout L = MatrixLayout::column>
    class Matrix {
    private:
        static_assert(N > 0);
        static_assert(L == MatrixLayout::row || L == MatrixLayout::column);
        template <typename M1, size_t N1> friend struct RS_Detail::Inverse;
        static constexpr MatrixLayout alt_layout = L == MatrixLayout::column ? MatrixLayout::row : MatrixLayout::column;
        using alt_matrix = Matrix<T, N, alt_layout>;
        using layout_specific = RS_Detail::LayoutSpecific<T, N, L>;
        std::array<T, N * N> arr;
        static void range_check(size_t r, size_t c) { if (r >= N || c >= N) throw std::out_of_range("Matrix index is out of range"); }
    public:
        using value_type = T;
        using vector_type = Vector<T, N>;
        static constexpr size_t dim = N;
        static constexpr size_t cells = N * N;
        static constexpr MatrixLayout layout = L;
        Matrix() noexcept { std::fill(begin(), end(), T(0)); }
        template <typename T2> explicit Matrix(T2 t) noexcept { std::fill(begin(), end(), t); }
        template <typename T2, typename T3> Matrix(T2 lead, T3 other) noexcept;
        Matrix(const alt_matrix& m) noexcept;
        template <typename T2> Matrix(std::initializer_list<T2> ts) noexcept;
        Matrix& operator=(const alt_matrix& m) noexcept;
        template <typename T2> Matrix& operator=(std::initializer_list<T2> ts) noexcept;
        Matrix operator+() const noexcept { return *this; }
        Matrix operator-() const noexcept;
        Matrix& operator+=(const Matrix& rhs) noexcept;
        Matrix& operator+=(const alt_matrix& rhs) noexcept;
        Matrix& operator-=(const Matrix& rhs) noexcept;
        Matrix& operator-=(const alt_matrix& rhs) noexcept;
        template <typename T2> Matrix& operator*=(T2 rhs) noexcept;
        template <typename T2> Matrix& operator/=(T2 rhs) noexcept;
        T& operator()(size_t r, size_t c) noexcept { return layout_specific::get_ref(arr.data(), r, c); }
        T operator()(size_t r, size_t c) const noexcept { return layout_specific::get_ref(arr.data(), r, c); }
        T& at(size_t r, size_t c) { range_check(r, c); return layout_specific::get_ref(arr.data(), r, c); }
        T at(size_t r, size_t c) const { range_check(r, c); return layout_specific::get_ref(arr.data(), r, c); }
        T* begin() noexcept { return arr.data(); }
        const T* begin() const noexcept { return arr.data(); }
        T* end() noexcept { return begin() + cells; }
        const T* end() const noexcept { return begin() + cells; }
        vector_type column(size_t c) const noexcept { return layout_specific::get_column(arr.data(), c); }
        vector_type row(size_t r) const noexcept { return layout_specific::get_row(arr.data(), r); }
        T det() const noexcept { return RS_Detail::Determinant<Matrix>()(*this); }
        Matrix inverse() const noexcept { return RS_Detail::Inverse<Matrix>()(*this); }
        constexpr size_t size() const noexcept { return cells; }
        Matrix swap_columns(size_t c1, size_t c2) const noexcept;
        Matrix swap_rows(size_t r1, size_t r2) const noexcept;
        Matrix transpose() const noexcept;
        static Matrix from_array(const T* ptr) noexcept;
        template <typename... Args> static Matrix from_columns(Args... args) noexcept;
        template <typename... Args> static Matrix from_rows(Args... args) noexcept;
        static Matrix identity() noexcept { return Matrix(T(1), T(0)); }
        static Matrix zero() noexcept { return {}; }
    };

    using Float2x2 = Matrix<float, 2>;
    using Float3x3 = Matrix<float, 3>;
    using Float4x4 = Matrix<float, 4>;
    using Float2x2c = Matrix<float, 2, MatrixLayout::column>;
    using Float3x3c = Matrix<float, 3, MatrixLayout::column>;
    using Float4x4c = Matrix<float, 4, MatrixLayout::column>;
    using Float2x2r = Matrix<float, 2, MatrixLayout::row>;
    using Float3x3r = Matrix<float, 3, MatrixLayout::row>;
    using Float4x4r = Matrix<float, 4, MatrixLayout::row>;
    using Double2x2 = Matrix<double, 2>;
    using Double3x3 = Matrix<double, 3>;
    using Double4x4 = Matrix<double, 4>;
    using Double2x2c = Matrix<double, 2, MatrixLayout::column>;
    using Double3x3c = Matrix<double, 3, MatrixLayout::column>;
    using Double4x4c = Matrix<double, 4, MatrixLayout::column>;
    using Double2x2r = Matrix<double, 2, MatrixLayout::row>;
    using Double3x3r = Matrix<double, 3, MatrixLayout::row>;
    using Double4x4r = Matrix<double, 4, MatrixLayout::row>;
    using Ldouble2x2 = Matrix<long double, 2>;
    using Ldouble3x3 = Matrix<long double, 3>;
    using Ldouble4x4 = Matrix<long double, 4>;
    using Ldouble2x2c = Matrix<long double, 2, MatrixLayout::column>;
    using Ldouble3x3c = Matrix<long double, 3, MatrixLayout::column>;
    using Ldouble4x4c = Matrix<long double, 4, MatrixLayout::column>;
    using Ldouble2x2r = Matrix<long double, 2, MatrixLayout::row>;
    using Ldouble3x3r = Matrix<long double, 3, MatrixLayout::row>;
    using Ldouble4x4r = Matrix<long double, 4, MatrixLayout::row>;

    template <typename T, size_t N, MatrixLayout L>
    template <typename T2, typename T3>
    Matrix<T, N, L>::Matrix(T2 lead, T3 other) noexcept {
        std::fill(begin(), end(), other);
        for (size_t i = 0; i < cells; i += N + 1)
            arr[i] = lead;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>::Matrix(const alt_matrix& m) noexcept {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                (*this)(r, c) = m(r, c);
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename T2>
    Matrix<T, N, L>::Matrix(std::initializer_list<T2> ts) noexcept {
        size_t nts = std::min(cells, ts.size());
        std::copy(ts.begin(), ts.begin() + nts, begin());
        std::fill(begin() + nts, end(), T(0));
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>& Matrix<T, N, L>::operator=(const alt_matrix& m) noexcept {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                (*this)(r, c) = m(r, c);
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename T2>
    Matrix<T, N, L>& Matrix<T, N, L>::operator=(std::initializer_list<T2> ts) noexcept {
        size_t nts = std::min(cells, ts.size());
        std::copy(ts.begin(), ts.begin() + nts, begin());
        std::fill(begin() + nts, end(), T(0));
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L> Matrix<T, N, L>::operator-() const noexcept {
        Matrix m;
        std::transform(begin(), end(), m.begin(), [] (T t) { return - t; });
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>& Matrix<T, N, L>::operator+=(const Matrix& rhs) noexcept {
        T* p = begin();
        T* endp = end();
        const T* q = rhs.begin();
        for (; p != endp; ++p, ++q)
            *p += *q;
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>& Matrix<T, N, L>::operator+=(const alt_matrix& rhs) noexcept {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                (*this)(r, c) += rhs(r, c);
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>& Matrix<T, N, L>::operator-=(const Matrix& rhs) noexcept {
        T* p = begin();
        T* endp = end();
        const T* q = rhs.begin();
        for (; p != endp; ++p, ++q)
            *p -= *q;
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L>& Matrix<T, N, L>::operator-=(const alt_matrix& rhs) noexcept {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                (*this)(r, c) -= rhs(r, c);
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename T2>
    Matrix<T, N, L>& Matrix<T, N, L>::operator*=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t *= rhs; });
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename T2>
    Matrix<T, N, L>& Matrix<T, N, L>::operator/=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t /= rhs; });
        return *this;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L> Matrix<T, N, L>::swap_columns(size_t c1, size_t c2) const noexcept {
        Matrix m = *this;
        layout_specific::swap_columns(m.arr.data(), c1, c2);
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L> Matrix<T, N, L>::swap_rows(size_t r1, size_t r2) const noexcept {
        Matrix m = *this;
        layout_specific::swap_rows(m.arr.data(), r1, r2);
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L> Matrix<T, N, L>::transpose() const noexcept {
        Matrix m;
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                m(r, c) = (*this)(c, r);
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    Matrix<T, N, L> Matrix<T, N, L>::from_array(const T* ptr) noexcept {
        Matrix m;
        std::copy_n(ptr, cells, m.arr.data());
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename... Args>
    Matrix<T, N, L> Matrix<T, N, L>::from_columns(Args... args) noexcept {
        T array[] = {T(args)...};
        auto i = std::begin(array), e = std::end(array);
        Matrix m;
        for (size_t c = 0; c < N && i != e; ++c)
            for (size_t r = 0; r < N && i != e; ++r)
                m(r, c) = *i++;
        return m;
    }

    template <typename T, size_t N, MatrixLayout L>
    template <typename... Args>
    Matrix<T, N, L> Matrix<T, N, L>::from_rows(Args... args) noexcept {
        T array[] = {T(args)...};
        auto i = std::begin(array), e = std::end(array);
        Matrix m;
        for (size_t r = 0; r < N && i != e; ++r)
            for (size_t c = 0; c < N && i != e; ++c)
                m(r, c) = *i++;
        return m;
    }

    template <typename T, size_t N, MatrixLayout L1, MatrixLayout L2>
    Matrix<T, N, L1> operator+(const Matrix<T, N, L1>& lhs, const Matrix<T, N, L2>& rhs) noexcept {
        auto m = lhs;
        m += rhs;
        return m;
    }

    template <typename T, size_t N, MatrixLayout L1, MatrixLayout L2>
    Matrix<T, N, L1> operator-(const Matrix<T, N, L1>& lhs, const Matrix<T, N, L2>& rhs) noexcept {
        auto m = lhs;
        m -= rhs;
        return m;
    }

    template <typename T, size_t N, MatrixLayout L1, MatrixLayout L2>
    Matrix<T, N, L1> operator*(const Matrix<T, N, L1>& lhs, const Matrix<T, N, L2>& rhs) noexcept {
        using layout_specific = RS_Detail::LayoutSpecific<T, N, L1>;
        Matrix<T, N, L1> m;
        auto ptr = const_cast<T*>(m.begin());
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                for (size_t i = 0; i < N; ++i)
                    layout_specific::get_ref(ptr, r, c) += lhs(r, i) * rhs(i, c);
        return m;
    }

    template <typename T1, typename T2, size_t N, MatrixLayout L>
    Vector<T2, N> operator*(const Matrix<T1, N, L>& lhs, const Vector<T2, N>& rhs) noexcept {
        Vector<T2, N> v;
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                v[r] += lhs(r, c) * rhs[c];
        return v;
    }

    template <typename T1, typename T2, size_t N, MatrixLayout L>
    Vector<T1, N> operator*(const Vector<T1, N>& lhs, const Matrix<T2, N, L>& rhs) noexcept {
        Vector<T1, N> v;
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                v[c] += lhs[r] * rhs(r, c);
        return v;
    }

    template <typename T1, typename T2, size_t N, MatrixLayout L>
    Matrix<T1, N, L> operator*(const Matrix<T1, N, L>& lhs, T2 rhs) noexcept {
        auto m = lhs;
        m *= rhs;
        return m;
    }

    template <typename T1, typename T2, size_t N, MatrixLayout L>
    Matrix<T2, N, L> operator*(T1 lhs, const Matrix<T2, N, L>& rhs) noexcept {
        auto m = rhs;
        m *= lhs;
        return m;
    }

    template <typename T1, typename T2, size_t N, MatrixLayout L>
    Matrix<T1, N, L> operator/(const Matrix<T1, N, L>& lhs, T2 rhs) noexcept {
        auto m = lhs;
        m /= rhs;
        return m;
    }

    template <typename T, size_t N, MatrixLayout L1, MatrixLayout L2>
    bool operator==(const Matrix<T, N, L1>& lhs, const Matrix<T, N, L2>& rhs) noexcept {
        for (size_t r = 0; r < N; ++r)
            for (size_t c = 0; c < N; ++c)
                if (! (lhs(r, c) == rhs(r, c)))
                    return false;
        return true;
    }

    template <typename T, size_t N, MatrixLayout L>
    bool operator==(const Matrix<T, N, L>& lhs, const Matrix<T, N, L>& rhs) noexcept {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, size_t N, MatrixLayout L1, MatrixLayout L2>
    bool operator!=(const Matrix<T, N, L1>& lhs, const Matrix<T, N, L2>& rhs) noexcept {
        return ! (lhs == rhs);
    }

    template <typename T, size_t N, MatrixLayout L>
    Ustring to_str(const Matrix<T, N, L>& m) {
        Ustring s;
        for (size_t r = 0; r < N; ++r) {
            s += r ? ',' : '[';
            for (size_t c = 0; c < N; ++c) {
                s += c ? ',' : '[';
                s += to_str(m(r, c));
            }
            s += ']';
        }
        return s += ']';
    }

    template <typename T, size_t N, MatrixLayout L>
    std::ostream& operator<<(std::ostream& out, const Matrix<T, N, L>& m) {
        return out << to_str(m);
    }

    // Quaternion

    template <typename T>
    class Quaternion:
    public EqualityComparable<Quaternion<T>> {
    public:
        using value_type = T;
        Quaternion() noexcept: arr{{T(0), T(0), T(0), T(0)}} {}
        Quaternion(T a) noexcept: arr{{a, T(0), T(0), T(0)}} {}
        Quaternion(T a, T b, T c, T d) noexcept: arr{{a, b, c, d}} {}
        Quaternion(T a, const Vector<T, 3>& bcd) noexcept: arr{{a, bcd.x(), bcd.y(), bcd.z()}} {}
        T& operator[](size_t i) noexcept { return arr[i]; }
        const T& operator[](size_t i) const noexcept { return arr[i]; }
        Quaternion operator+() const noexcept { return *this; }
        Quaternion operator-() const noexcept { return {- a(), b(), c(), d()}; }
        Quaternion& operator+=(const Quaternion& rhs) noexcept;
        Quaternion& operator-=(const Quaternion& rhs) noexcept;
        template <typename T2> Quaternion& operator*=(T2 rhs) noexcept;
        template <typename T2> Quaternion& operator/=(T2 rhs) noexcept;
        Quaternion& operator*=(const Quaternion& rhs) noexcept;
        T& a() noexcept { return arr[0]; }
        const T& a() const noexcept { return arr[0]; }
        T& b() noexcept { return arr[1]; }
        const T& b() const noexcept { return arr[1]; }
        T& c() noexcept { return arr[2]; }
        const T& c() const noexcept { return arr[2]; }
        T& d() noexcept { return arr[3]; }
        const T& d() const noexcept { return arr[3]; }
        T* begin() noexcept { return arr.data(); }
        const T* begin() const noexcept { return arr.data(); }
        T* end() noexcept { return begin() + 4; }
        const T* end() const noexcept { return begin() + 4; }
        Quaternion conj() const noexcept { return {a(), - b(), - c(), - d()}; }
        Quaternion conj(const Quaternion& p) const noexcept { return *this * p * conj(); }
        T norm() const noexcept { return std::sqrt(norm2()); }
        T norm2() const noexcept { return std::inner_product(begin(), end(), begin(), T(0)); }
        Quaternion recip() const noexcept { return conj() / norm2(); }
        T s_part() const noexcept { return a(); }
        Vector<T, 3> v_part() const noexcept { return {b(), c(), d()}; }
        Quaternion versor() const noexcept { return *this / norm(); }
        Vector<T, 4> to_vector() const noexcept { return {a(), b(), c(), d()}; }
        static Quaternion from_vector(const Vector<T, 4>& v) noexcept { return {v.x(), v.y(), v.z(), v.w()}; }
    private:
        std::array<T, 4> arr;
    };

    using Qfloat = Quaternion<float>;
    using Qdouble = Quaternion<double>;
    using Qldouble = Quaternion<long double>;

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator+=(const Quaternion<T>& rhs) noexcept {
        for (size_t i = 0; i < 4; ++i)
            arr[i] += rhs.arr[i];
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator-=(const Quaternion<T>& rhs) noexcept {
        for (size_t i = 0; i < 4; ++i)
            arr[i] -= rhs.arr[i];
        return *this;
    }

    template <typename T>
    Quaternion<T> operator+(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        auto q = lhs;
        q += rhs;
        return q;
    }

    template <typename T>
    Quaternion<T> operator-(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        auto q = lhs;
        q -= rhs;
        return q;
    }

    template <typename T>
    template <typename T2>
    Quaternion<T>& Quaternion<T>::operator*=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t *= rhs; });
        return *this;
    }

    template <typename T>
    template <typename T2>
    Quaternion<T>& Quaternion<T>::operator/=(T2 rhs) noexcept {
        std::for_each(begin(), end(), [rhs] (T& t) { t /= rhs; });
        return *this;
    }

    template <typename T1, typename T2>
    Quaternion<T1> operator*(const Quaternion<T1>& lhs, T2 rhs) noexcept {
        auto q = lhs;
        q *= rhs;
        return q;
    }

    template <typename T1, typename T2>
    Quaternion<T2> operator*(T1 lhs, const Quaternion<T2>& rhs) noexcept {
        auto q = rhs;
        q *= lhs;
        return q;
    }

    template <typename T1, typename T2>
    Quaternion<T1> operator/(const Quaternion<T1>& lhs, T2 rhs) noexcept {
        auto q = lhs;
        q /= rhs;
        return q;
    }

    template <typename T>
    Quaternion<T> operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        return {
            lhs.a() * rhs.a() - lhs.b() * rhs.b() - lhs.c() * rhs.c() - lhs.d() * rhs.d(),
            lhs.a() * rhs.b() + lhs.b() * rhs.a() + lhs.c() * rhs.d() - lhs.d() * rhs.c(),
            lhs.a() * rhs.c() - lhs.b() * rhs.d() + lhs.c() * rhs.a() + lhs.d() * rhs.b(),
            lhs.a() * rhs.d() + lhs.b() * rhs.c() - lhs.c() * rhs.b() + lhs.d() * rhs.a()
        };
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<T>& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    template <typename T>
    bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const Quaternion<T>& q) {
        return out << to_str(q);
    }

    // Coordinate transformations

    template <typename T>
    Vector<T, 2> cartesian_to_polar(const Vector<T, 2>& xy) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {xy.r(), std::atan2(xy[1], xy[0])};
    }

    template <typename T>
    Vector<T, 2> polar_to_cartesian(const Vector<T, 2>& rt) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {rt[0] * std::cos(rt[1]), rt[0] * std::sin(rt[1])};
    }

    template <typename T>
    Vector<T, 3> cartesian_to_cylindrical(const Vector<T, 3>& xyz) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {std::sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]), std::atan2(xyz[1], xyz[0]), xyz[2]};
    }

    template <typename T>
    Vector<T, 3> cartesian_to_spherical(const Vector<T, 3>& xyz) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        T rho = std::sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]);
        return {xyz.r(), std::atan2(xyz[1], xyz[0]), std::atan2(rho, xyz[2])};
    }

    template <typename T>
    Vector<T, 3> cylindrical_to_cartesian(const Vector<T, 3>& rpz) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {rpz[0] * std::cos(rpz[1]), rpz[0] * std::sin(rpz[1]), rpz[2]};
    }

    template <typename T>
    Vector<T, 3> cylindrical_to_spherical(const Vector<T, 3>& rpz) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {std::sqrt(rpz[0] * rpz[0] + rpz[2] * rpz[2]), rpz[1], std::atan2(rpz[0], rpz[2])};
    }

    template <typename T>
    Vector<T, 3> spherical_to_cartesian(const Vector<T, 3>& rpt) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        T rho = rpt[0] * std::sin(rpt[2]);
        return {rho * std::cos(rpt[1]), rho * std::sin(rpt[1]), rpt[0] * std::cos(rpt[2])};
    }

    template <typename T>
    Vector<T, 3> spherical_to_cylindrical(const Vector<T, 3>& rpt) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {rpt[0] * std::sin(rpt[2]), rpt[1], rpt[0] * std::cos(rpt[2])};
    }

    // Projective geometry

    template <typename T>
    inline Vector<T, 4> make4(const Vector<T, 3>& v, T w) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {v.x(), v.y(), v.z(), w};
    }

    template <typename T>
    inline Vector<T, 4> point4(const Vector<T, 3>& v) noexcept {
        return make4(v, T(1));
    }

    template <typename T>
    inline Vector<T, 4> norm4(const Vector<T, 3>& v) noexcept {
        return make4(v, T(0));
    }

    template <typename T>
    inline Vector<T, 3> point3(const Vector<T, 4>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Vector<T, 3> u{v.x(), v.y(), v.z()};
        if (v.w() != T(0))
            u /= v.w();
        return u;
    }

    template <typename T>
    inline Vector<T, 3> norm3(const Vector<T, 4>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return {v.x(), v.y(), v.z()};
    }

    template <typename T, MatrixLayout L>
    Matrix<T, 4, L> make_transform(const Matrix<T, 3, L>& m, const Vector<T, 3>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 4, L> t;
        for (size_t r = 0; r < 3; ++r) {
            for (size_t c = 0; c < 3; ++c)
                t(r, c) = m(r, c);
            t(r, 3) = v[r];
        }
        t(3, 0) = T(0);
        t(3, 1) = T(0);
        t(3, 2) = T(0);
        t(3, 3) = T(1);
        return t;
    }

    template <typename T, MatrixLayout L>
    inline Matrix<T, 4, L> normal_transform(const Matrix<T, 4, L>& m) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return m.inverse().transpose();
    }

    // Primitive transformations

    template <typename T>
    Matrix<T, 3> rotate3(T angle, size_t index) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        auto c = std::cos(angle), s = std::sin(angle);
        auto m = Matrix<T, 3>::identity();
        switch (index) {
            case 0: m(1,1) = c; m(1,2) = -s; m(2,1) = s; m(2,2) = c; break;
            case 1: m(0,0) = c; m(0,2) = s; m(2,0) = -s; m(2,2) = c; break;
            case 2: m(0,0) = c; m(0,1) = -s; m(1,0) = s; m(1,1) = c; break;
            default: break;
        }
        return m;
    }

    template <typename T>
    Matrix<T, 4> rotate4(T angle, size_t index) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        auto c = std::cos(angle), s = std::sin(angle);
        auto m = Matrix<T, 4>::identity();
        switch (index) {
            case 0: m(1,1) = c; m(1,2) = -s; m(2,1) = s; m(2,2) = c; break;
            case 1: m(0,0) = c; m(0,2) = s; m(2,0) = -s; m(2,2) = c; break;
            case 2: m(0,0) = c; m(0,1) = -s; m(1,0) = s; m(1,1) = c; break;
            default: break;
        }
        return m;
    }

    template <typename T>
    Matrix<T, 3> scale3(T t) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return Matrix<T, 3>(t, T(0));
    }

    template <typename T>
    Matrix<T, 3> scale3(const Vector<T, 3>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 3> m;
        for (size_t i = 0; i < 3; ++i)
            m(i, i) = v[i];
        return m;
    }

    template <typename T>
    Matrix<T, 4> scale4(T t) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 4> m(t, T(0));
        m(3, 3) = T(1);
        return m;
    }

    template <typename T>
    Matrix<T, 4> scale4(const Vector<T, 3>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 4> m;
        for (size_t i = 0; i < 3; ++i)
            m(i, i) = v[i];
        m(3, 3) = T(1);
        return m;
    }

    template <typename T>
    Matrix<T, 4> translate4(const Vector<T, 3>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        auto m = Matrix<T, 4>::identity();
        for (size_t i = 0; i < 3; ++i)
            m(i, 3) = v[i];
        return m;
    }

    // Quaternion transformations

    namespace RS_Detail {

        template <typename T, size_t N>
        void build_matrix(const Quaternion<T>& q, Matrix<T, N>& m) noexcept {
            m(0, 0) = q.a() * q.a() + q.b() * q.b() - q.c() * q.c() - q.d() * q.d();
            m(0, 1) = T(2) * q.b() * q.c() - T(2) * q.a() * q.d();
            m(0, 2) = T(2) * q.a() * q.c() + T(2) * q.b() * q.d();
            m(1, 0) = T(2) * q.a() * q.d() + T(2) * q.b() * q.c();
            m(1, 1) = q.a() * q.a() - q.b() * q.b() + q.c() * q.c() - q.d() * q.d();
            m(1, 2) = T(2) * q.c() * q.d() - T(2) * q.a() * q.b();
            m(2, 0) = T(2) * q.b() * q.d() - T(2) * q.a() * q.c();
            m(2, 1) = T(2) * q.a() * q.b() + T(2) * q.c() * q.d();
            m(2, 2) = q.a() * q.a() - q.b() * q.b() - q.c() * q.c() + q.d() * q.d();
        }

    }

    template <typename T>
    Vector<T, 3> rotate(const Quaternion<T>& q, const Vector<T, 3>& v) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return q.conj({T(0), v}).v_part();
    }

    template <typename T>
    Quaternion<T> rotateq(T angle, const Vector<T, 3>& axis) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        if (axis == Vector<T, 3>())
            return T(1);
        angle /= T(2);
        return {std::cos(angle), std::sin(angle) * axis.dir()};
    }

    template <typename T>
    Matrix<T, 3> rotate3(const Quaternion<T>& q) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 3> m;
        RS_Detail::build_matrix(q, m);
        return m;
    }

    template <typename T>
    Matrix<T, 3> rotate3(T angle, const Vector<T, 3>& axis) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return rotate3(rotateq(angle, axis));
    }

    template <typename T>
    Matrix<T, 4> rotate4(const Quaternion<T>& q) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        Matrix<T, 4> m;
        RS_Detail::build_matrix(q, m);
        m(3, 3) = T(1);
        return m;
    }

    template <typename T>
    Matrix<T, 4> rotate4(T angle, const Vector<T, 3>& axis) noexcept {
        static_assert(std::is_floating_point<T>::value, "Only floating point coordinates are supported");
        return rotate4(rotateq(angle, axis));
    }

}

namespace std {

    template <typename T, size_t N>
    struct hash<RS::Vector<T, N>> {
        using argument_type = RS::Vector<T, N>;
        using result_type = size_t;
        size_t operator()(const RS::Vector<T, N>& v) const noexcept { return RS::hash_range(v); }
    };

    template <typename T, size_t N, RS::MatrixLayout L>
    struct hash<RS::Matrix<T, N, L>> {
        using argument_type = RS::Matrix<T, N, L>;
        using result_type = size_t;
        size_t operator()(const RS::Matrix<T, N, L>& m) const noexcept { return RS::hash_range(m); }
    };

    template <typename T>
    struct hash<RS::Quaternion<T>> {
        using argument_type = RS::Quaternion<T>;
        using result_type = size_t;
        size_t operator()(const RS::Quaternion<T>& q) const noexcept { return RS::hash_range(q); }
    };

}

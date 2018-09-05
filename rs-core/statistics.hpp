#pragma once

#include "rs-core/common.hpp"
#include <cmath>
#include <type_traits>
#include <utility>

namespace RS {

    template <typename T = double>
    class Statistics {
    public:
        using scalar_type = T;
        Statistics() = default;
        void add(T x, T y = 0) noexcept;
        void add(const std::pair<T, T>& xy) noexcept { add(xy.first, xy.second); }
        template <typename InputRange> void append(const InputRange& r) { for (auto& t: r) add(t); }
        void clear() noexcept { xmin = ymin = xmax = ymax = xsum = x2sum = xysum = ysum = y2sum = 0; n = 0; }
        bool empty() const noexcept { return n == 0; }
        size_t num() const noexcept { return n; }
        T t_num() const noexcept { return T(n); }
        T min() const noexcept { return xmin; }
        T min_x() const noexcept { return xmin; }
        T min_y() const noexcept { return ymin; }
        T max() const noexcept { return xmax; }
        T max_x() const noexcept { return xmax; }
        T max_y() const noexcept { return ymax; }
        T mean() const noexcept { return mean_x(); }
        T mean_x() const noexcept { return n ? xsum / T(n) : T(0); }
        T mean_y() const noexcept { return n ? ysum / T(n) : T(0); }
        T stdevp() const noexcept { return stdevp_x(); }
        T stdevp_x() const noexcept { return get_stdevp(n, xsum, x2sum); }
        T stdevp_y() const noexcept { return get_stdevp(n, ysum, y2sum); }
        T stdevs() const noexcept { return stdevs_x(); }
        T stdevs_x() const noexcept { return get_stdevs(n, xsum, x2sum); }
        T stdevs_y() const noexcept { return get_stdevs(n, ysum, y2sum); }
        T correlation() const noexcept;
        std::pair<T, T> linear() const noexcept { return linear_xy(); }
        std::pair<T, T> linear_xy() const noexcept;
        std::pair<T, T> linear_yx() const noexcept;
    private:
        static_assert(std::is_floating_point<T>::value);
        T xmin = 0;
        T ymin = 0;
        T xmax = 0;
        T ymax = 0;
        T xsum = 0;
        T x2sum = 0;
        T xysum = 0;
        T ysum = 0;
        T y2sum = 0;
        size_t n = 0;
        static T get_stdevs(size_t n, T sum, T sum2) noexcept;
        static T get_stdevp(size_t n, T sum, T sum2) noexcept;
    };

    template <typename T>
    void Statistics<T>::add(T x, T y) noexcept {
        xmin = n ? std::min(xmin, x) : x;
        ymin = n ? std::min(ymin, y) : y;
        xmax = n ? std::max(xmax, x) : x;
        ymax = n ? std::max(ymax, y) : y;
        xsum += x;
        x2sum += x * x;
        xysum += x * y;
        ysum += y;
        y2sum += y * y;
        ++n;
    }

    template <typename T>
    T Statistics<T>::correlation() const noexcept {
        if (n < 2)
            return 0;
        T a = T(n) * xysum - xsum * ysum;
        T b = T(n) * x2sum - xsum * xsum;
        T c = T(n) * y2sum - ysum * ysum;
        if (b == 0 || c == 0)
            return 0;
        else
            return a / std::sqrt(b * c);
    }

    template <typename T>
    std::pair<T, T> Statistics<T>::linear_xy() const noexcept {
        if (n == 0)
            return {T(0), T(0)};
        else if (n == 1)
            return {T(0), ysum};
        T divisor = T(n) * x2sum - xsum * xsum;
        if (divisor == 0)
            return {T(0), mean_y()};
        T a = (T(n) * xysum - xsum * ysum) / divisor;
        T b = (ysum - a * xsum) / T(n);
        return {a, b};
    }

    template <typename T>
    std::pair<T, T> Statistics<T>::linear_yx() const noexcept {
        if (n == 0)
            return {T(0), T(0)};
        else if (n == 1)
            return {T(0), xsum};
        T divisor = T(n) * y2sum - ysum * ysum;
        if (divisor == 0)
            return {T(0), mean_x()};
        T a = (T(n) * xysum - xsum * ysum) / divisor;
        T b = (xsum - a * ysum) / T(n);
        return {a, b};
    }

    template <typename T>
    T Statistics<T>::get_stdevs(size_t n, T sum, T sum2) noexcept {
        return n >= 2 ? std::sqrt((sum2 - sum * sum / T(n)) / (T(n) - T(1))) : T(0);
    }

    template <typename T>
    T Statistics<T>::get_stdevp(size_t n, T sum, T sum2) noexcept {
        return n ? std::sqrt((sum2 - sum * sum / T(n)) / T(n)) : T(0);
    }

}

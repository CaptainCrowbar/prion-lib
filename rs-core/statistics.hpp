#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    template <typename T>
    class Statistics {
    public:
        static_assert(std::is_floating_point_v<T>);
        using scalar_type = T;
        Statistics() = default;
        explicit Statistics(size_t columns);
        template <typename... Args> void operator()(Args... args) { i(args...); }
        template <typename... Args> void i(Args... args) { w(T(1), args...); }
        template <size_t N> void w(T wt, const std::array<T, N>& array);
        template <typename T1, typename T2> void w(T wt, const std::pair<T1, T2> pair) { w(wt, tuple_to_array(pair)); }
        template <typename... TS> void w(T wt, const std::tuple<TS...>& tuple) { w(wt, tuple_to_array(tuple)); }
        template <typename... Args> void w(T wt, Args... args) { w(wt, std::array<std::common_type_t<Args...>, sizeof...(Args)>{{args...}}); }
        size_t columns() const noexcept { return min_.size(); }
        size_t count() const noexcept { return count_; }
        bool empty() const noexcept { return count_ == 0; }
        T weight() const noexcept { return wsum_; }
        T min(size_t i = 0) const noexcept { return min_[i]; }
        T max(size_t i = 0) const noexcept { return max_[i]; }
        T sum(size_t i = 0) const noexcept { return sum_[i]; }
        T mean(size_t i = 0) const noexcept { return mean_[i]; }
        T variance(size_t i = 0) const noexcept { return vsum_[i] / wsum_; }
        T variance_bc(size_t i = 0) const noexcept { return vsum_[i] / (wsum_ - T(1)); }
        T variance_bc_rel(size_t i = 0) const noexcept { return vsum_[i] / (wsum_ - wsum2_ / wsum_); }
        T sd(size_t i = 0) const noexcept { return std::sqrt(variance(i)); }
        T sd_bc(size_t i = 0) const noexcept { return std::sqrt(variance_bc(i)); }
        T sd_bc_rel(size_t i = 0) const noexcept { return std::sqrt(variance_bc_rel(i)); }
        T covariance() const noexcept { return covariance(0, 1); }
        T covariance(size_t i, size_t j) const noexcept;
        T covariance_bc() const noexcept { return covariance_bc(0, 1); }
        T covariance_bc(size_t i, size_t j) const noexcept;
        T covariance_bc_rel() const noexcept { return covariance_bc_rel(0, 1); }
        T covariance_bc_rel(size_t i, size_t j) const noexcept;
        T correlation() const noexcept { return correlation(0, 1); }
        T correlation(size_t i, size_t j) const noexcept;
        std::pair<T, T> linear() const noexcept { return linear(0, 1); }
        std::pair<T, T> linear(size_t i, size_t j) const noexcept;
        void clear() noexcept;
        void clear(size_t columns) noexcept;
        void swap(Statistics& s) noexcept;
        friend void swap(Statistics& s1, Statistics& s2) noexcept { s1.swap(s2); }
    private:
        std::vector<T> min_;
        std::vector<T> max_;
        std::vector<T> sum_;
        std::vector<T> sum2_;
        std::vector<T> mean_;
        std::vector<T> vsum_;
        std::vector<T> sumxy_;
        std::vector<T> csum_;
        T wsum_ = T(0);
        T wsum2_ = T(0);
        size_t count_ = 0;
        static constexpr size_t xy_index(size_t i, size_t j) noexcept { return i < j ? j * (j - 1) / 2 + i : xy_index(j, i); }
        static constexpr size_t xy_size(size_t n) noexcept { return n * (n + 1) / 2; }
    };

    template <typename T>
    Statistics<T>::Statistics(size_t columns):
    min_(columns, T(0)),
    max_(columns, T(0)),
    sum_(columns, T(0)),
    sum2_(columns, T(0)),
    mean_(columns, T(0)),
    vsum_(columns, T(0)),
    sumxy_(xy_size(columns), T(0)),
    csum_(xy_size(columns), T(0)),
    wsum_{T{0}},
    wsum2_{T{0}},
    count_(0) {}

    template <typename T>
    template <size_t N>
    void Statistics<T>::w(T wt, const std::array<T, N>& array) {
        size_t old_size = min_.size();
        for (size_t i = 0; i < old_size; ++i) {
            min_[i] = std::min(min_[i], array[i]);
            max_[i] = std::max(max_[i], array[i]);
        }
        if (N > old_size) {
            min_.resize(N, T(0));
            max_.resize(N, T(0));
            sum_.resize(N, T(0));
            sum2_.resize(N, T(0));
            mean_.resize(N, T(0));
            vsum_.resize(N, T(0));
            sumxy_.resize(xy_size(N), T(0));
            csum_.resize(xy_size(N), T(0));
        }
        std::copy(array.begin() + old_size, array.end(), min_.begin() + old_size);
        std::copy(array.begin() + old_size, array.end(), max_.begin() + old_size);
        ++count_;
        wsum_ += wt;
        wsum2_ += wt * wt;
        for (size_t i = 0; i < N; ++i) {
            sum_[i] += wt * array[i];
            sum2_[i] += wt * array[i] * array[i];
            T m = mean_[i];
            mean_[i] += (wt / wsum_) * (array[i] - m);
            vsum_[i] += wt * (array[i] - m) * (array[i] - mean_[i]);
            T dx = array[i] - mean_[i];
            for (size_t j = i + 1; j < N; ++j) {
                size_t k = xy_index(i, j);
                sumxy_[k] += wt * array[i] * array[j];
                csum_[i] += wt * dx * (array[j] - mean_[j]);
            }
        }
    }

    template <typename T>
    T Statistics<T>::covariance(size_t i, size_t j) const noexcept {
        if (i == j)
            return variance(i);
        else
            return csum_[xy_index(i, j)] / wsum_;
    }

    template <typename T>
    T Statistics<T>::covariance_bc(size_t i, size_t j) const noexcept {
        if (i == j)
            return variance_bc(i);
        else
            return csum_[xy_index(i, j)] / (wsum_ - T(1));
    }

    template <typename T>
    T Statistics<T>::covariance_bc_rel(size_t i, size_t j) const noexcept {
        if (i == j)
            return variance_bc_rel(i);
        else
            return csum_[xy_index(i, j)] / (wsum_ - wsum2_ / wsum_);
    }

    template <typename T>
    T Statistics<T>::correlation(size_t i, size_t j) const noexcept {
        return covariance(i, j) / std::sqrt(variance(i) * variance(j));
    }

    template <typename T>
    std::pair<T, T> Statistics<T>::linear(size_t i, size_t j) const noexcept {
        if (count_ <= T(1))
            return {T(0), sum_[j]};
        size_t k = xy_index(i, j);
        T divisor = wsum_ * sum2_[i] - sum_[i] * sum_[i];
        if (divisor == T(0))
            return {T(0), mean_[j]};
        T a = (wsum_ * sumxy_[k] - sum_[i] * sum_[j]) / divisor;
        T b = (sum_[j] - a * sum_[i]) / wsum_;
        return {a, b};
    }

    template <typename T>
    void Statistics<T>::clear() noexcept {
        min_.clear();
        max_.clear();
        sum_.clear();
        sum2_.clear();
        mean_.clear();
        vsum_.clear();
        sumxy_.clear();
        csum_.clear();
        wsum_ = wsum2_ = T(0);
        count_ = 0;
    }

    template <typename T>
    void Statistics<T>::clear(size_t columns) noexcept {
        Statistics s(columns);
        swap(s);
    }

    template <typename T>
    void Statistics<T>::swap(Statistics& s) noexcept {
        min_.swap(s.min_);
        max_.swap(s.max_);
        sum_.swap(s.sum_);
        sum2_.swap(s.sum2_);
        mean_.swap(s.mean_);
        vsum_.swap(s.vsum_);
        sumxy_.swap(s.sumxy_);
        csum_.swap(s.csum_);
        std::swap(wsum_, s.wsum_);
        std::swap(wsum2_, s.wsum2_);
        std::swap(count_, s.count_);
    }

}

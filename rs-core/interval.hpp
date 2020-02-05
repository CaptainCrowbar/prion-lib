#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/serial.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    // Supporting types

    RS_ENUM_CLASS(IntervalBound, int, 0,
        empty,     // The interval is empty
        closed,    // The interval includes the boundary value
        open,      // The interval does not include the boundary value
        unbound);  // The interval is unbounded in this direction

    constexpr IntervalBound operator~(IntervalBound b) noexcept { return IntervalBound(3 - int(b)); }

    RS_ENUM_CLASS(IntervalCategory, int, 0,
        none,         // Not usable in an interval
        ordered,      // Ordered but not an arithmetic type (e.g. string)
        integral,     // Incrementable and not continuous (e.g. integer)
        continuous);  // Models a continuous arithmetic type (e.g. floating point)

    RS_ENUM_CLASS(IntervalMatch, int, -2,
        empty,  // The interval is empty
        low,    // The value is less than the interval's lower bound
        ok,     // The value is an element of the interval
        high);  // The value is greater than the interval's upper bound

    RS_ENUM_CLASS(IntervalOrder, int, -7,
        // Name             Index  Picture    Description
        b_only,             // -7  bbb        A is empty, B is not
        a_below_b,          // -6  aaa...bbb  A's upper bound is less than B's lower bound, with a gap
        a_touches_b,        // -5  aaabbb     A's upper bound is less than B's lower bound, with no gap
        a_overlaps_b,       // -4  aaa###bbb  A's upper bound overlaps B's lower bound
        a_extends_below_b,  // -3  aaa###     B is a subset of A, with the same upper bound
        a_encloses_b,       // -2  aaa###aaa  B is a subset of A, matching neither bound
        b_extends_above_a,  // -1  ###bbb     A is a subset of B, with the same lower bound
        equal,              // 0   ###        A and B are the same (this includes the case where both are empty)
        a_extends_above_b,  // 1   ###aaa     B is a subset of A, with the same lower bound
        b_encloses_a,       // 2   bbb###bbb  A is a subset of B, matching neither bound
        b_extends_below_a,  // 3   bbb###     A is a subset of B, with the same upper bound
        b_overlaps_a,       // 4   bbb###aaa  B's upper bound overlaps A's lower bound
        b_touches_a,        // 5   bbbaaa     B's upper bound is less than A's lower bound, with no gap
        b_below_a,          // 6   bbb...aaa  B's upper bound is less than A's lower bound, with a gap
        a_only);            // 7   aaa        B is empty, A is not

    template <typename T>
    struct IntervalTraits {
        using base_type = std::remove_cv_t<T>;
        static constexpr IntervalCategory category =
            std::is_same_v<base_type, bool> ? IntervalCategory::none :
            ! std::is_default_constructible_v<base_type> ? IntervalCategory::none :
            ! std::is_copy_constructible_v<base_type> ? IntervalCategory::none :
            ! std::is_copy_assignable_v<base_type> ? IntervalCategory::none :
            ! Meta::has_comparison_operators<base_type> ? IntervalCategory::none :
            std::numeric_limits<base_type>::is_specialized ?
                (std::numeric_limits<base_type>::is_integer ? IntervalCategory::integral : IntervalCategory::continuous) :
            Meta::has_plus_operator<base_type> && Meta::has_minus_operator<base_type>
                && Meta::has_multiply_operator<base_type> && Meta::has_divide_operator<base_type>
                && ! Meta::has_remainder_operator<base_type> ? IntervalCategory::continuous :
            Meta::has_step_operators<base_type> ? IntervalCategory::integral :
            IntervalCategory::ordered;
    };

    template <typename T> constexpr auto interval_category = IntervalTraits<T>::category;

    // Common base class for all intervals

    class IntervalCommonBase {
    private:
        using IB = IntervalBound;
    protected:
        enum class boundary_type {
            minus_infinity,
            value_minus_epsilon,
            exact_value,
            value_plus_epsilon,
            plus_infinity
        };
        static constexpr IB invert_bound(IB b) noexcept;
        static constexpr bool no_boundary(boundary_type p) noexcept;
        static std::pair<IB, IB> parse_interval_mode(Uview mode);
        static bool parse_interval_string(Uview str, Uview& min, Uview& max, IB& left, IB& right) noexcept;
    };

        constexpr IntervalBound IntervalCommonBase::invert_bound(IB b) noexcept {
            if (b == IB::open)         return IB::closed;
            else if (b == IB::closed)  return IB::open;
            else                       return b;
        }

        constexpr bool IntervalCommonBase::no_boundary(boundary_type b) noexcept {
            return b == boundary_type::minus_infinity || b == boundary_type::plus_infinity;
        }

    // Base class for intervals with the same value type

    template <typename T>
    class IntervalTypeBase:
    public IntervalCommonBase {
    private:
        using IB = IntervalBound;
        using IC = IntervalCategory;
        using IM = IntervalMatch;
    public:
        const T& min() const noexcept { return min_; }
        const T& max() const noexcept { return max_; }
        IB left() const noexcept { return left_; }
        IB right() const noexcept { return right_; }
        bool empty() const noexcept { return left_ == IB::empty; }
        bool is_single() const noexcept { return left_ == IB::closed && right_ == IB::closed && min_ == max_; }
        bool is_finite() const noexcept { return is_left_bounded() && is_right_bounded(); }
        bool is_infinite() const noexcept { return left_ == IB::unbound || right_ == IB::unbound; }
        bool is_universal() const noexcept { return left_ == IB::unbound && right_ == IB::unbound; }
        bool is_left_bounded() const noexcept { return is_left_open() || is_left_closed(); }
        bool is_left_closed() const noexcept { return left_ == IB::closed; }
        bool is_left_open() const noexcept { return left_ == IB::open; }
        bool is_right_bounded() const noexcept { return is_right_open() || is_right_closed(); }
        bool is_right_closed() const noexcept { return right_ == IB::closed; }
        bool is_right_open() const noexcept { return right_ == IB::open; }
        void convert_from_json(const json& j);
        json convert_to_json() const;
        size_t hash() const noexcept;
        IM match(const T& t) const;
    protected:
        struct boundary_point {
            boundary_type flag;
            T value;
            bool operator<(const boundary_point& b) const noexcept
                { return no_boundary(flag) || no_boundary(b.flag) || value == b.value ? flag < b.flag : value < b.value; }
        };
        using boundary_points = std::pair<boundary_point, boundary_point>;
        T min_ = T();
        T max_ = T();
        IB left_ = IB::empty;
        IB right_ = IB::empty;
        void adjust_bounds();
        void do_swap(IntervalTypeBase& in) noexcept;
        boundary_points find_interval_bounds() const noexcept;
        static bool adjacent_intervals(const boundary_point& left, const boundary_point& right) noexcept;
    };

        template <typename T>
        void IntervalTypeBase<T>::convert_from_json(const json& j) {
            min_ = j[0];
            max_ = j[1];
            left_ = IB(int(j[2]));
            right_ = IB(int(j[3]));
        }

        template <typename T>
        json IntervalTypeBase<T>::convert_to_json() const {
            json j;
            j.push_back(min_);
            j.push_back(max_);
            j.push_back(int(left_));
            j.push_back(int(right_));
            return j;
        }

        template <typename T>
        size_t IntervalTypeBase<T>::hash() const noexcept {
            return hash_value(min_, max_, int(left_), int(right_));
        }

        template <typename T>
        IntervalMatch IntervalTypeBase<T>::match(const T& t) const {
            if (empty())                                return IM::empty;
            else if (is_universal())                    return IM::ok;
            else if (left_ == IB::closed && t < min_)   return IM::low;
            else if (left_ == IB::open && t <= min_)    return IM::low;
            else if (right_ == IB::closed && t > max_)  return IM::high;
            else if (right_ == IB::open && t >= max_)   return IM::high;
            else                                        return IM::ok;
        }

        template <typename T>
        void IntervalTypeBase<T>::adjust_bounds() {
            if ((left_ == IB::empty) != (right_ == IB::empty)) {
                throw std::invalid_argument("Inconsistent interval bounds");
            } else if (left_ == IB::empty) {
                min_ = max_ = T();
            } else {
                if (is_finite()) {
                    if (min_ > max_)
                        left_ = right_ = IB::empty;
                    else if (min_ == max_ && (left_ == IB::open || right_ == IB::open))
                        left_ = right_ = IB::empty;
                }
                if (! is_left_bounded())
                    min_ = T();
                if (! is_right_bounded())
                    max_ = T();
            }
        }

        template <typename T>
        void IntervalTypeBase<T>::do_swap(IntervalTypeBase& in) noexcept {
            using std::swap;
            swap(min_, in.min_);
            swap(max_, in.max_);
            swap(left_, in.left_);
            swap(right_, in.right_);
        }

        template <typename T>
        typename IntervalTypeBase<T>::boundary_points IntervalTypeBase<T>::find_interval_bounds() const noexcept {
            boundary_point lbp, rbp;
            if (left_ == IB::open)          lbp = {boundary_type::value_plus_epsilon, min_};
            else if (left_ == IB::closed)   lbp = {boundary_type::exact_value, min_};
            else                            lbp = {boundary_type::minus_infinity, {}};
            if (right_ == IB::open)         rbp = {boundary_type::value_minus_epsilon, max_};
            else if (right_ == IB::closed)  rbp = {boundary_type::exact_value, max_};
            else                            rbp = {boundary_type::plus_infinity, {}};
            return {lbp, rbp};
        };

        template <typename T>
        bool IntervalTypeBase<T>::adjacent_intervals(const boundary_point& left, const boundary_point& right) noexcept {
            if (IntervalCommonBase::no_boundary(left.flag) || IntervalCommonBase::no_boundary(right.flag))
                return false;
            if (int(right.flag) - int(left.flag) == 1 && left.value == right.value)
                return true;
            if constexpr (interval_category<T> == IC::integral) {
                if (left.flag == boundary_type::exact_value && right.flag == boundary_type::exact_value && left.value < right.value) {
                    T t = left.value;
                    return ++t == right.value;
                }
            }
            return false;
        };

    // Base class for intervals in the same category

    template <typename T, IntervalCategory Cat = interval_category<T>>
    class IntervalCategoryBase;

    template <typename T>
    class IntervalCategoryBase<T, IntervalCategory::ordered>:
    public IntervalTypeBase<T> {};

    template <typename T>
    class IntervalCategoryBase<T, IntervalCategory::integral>:
    public IntervalTypeBase<T> {
    private:
        using IB = IntervalBound;
        static constexpr bool has_t_arithmetic = Meta::has_plus_assign_operator<T, T> && Meta::has_minus_operator<T>;
        static constexpr bool has_int_arithmetic = Meta::has_plus_assign_operator<T, int> && Meta::has_minus_operator<T, int>;
        static constexpr bool has_random_access = has_t_arithmetic || has_int_arithmetic;
        static constexpr bool prefer_ptrdiff_arithmetic = has_int_arithmetic && sizeof(T) >= sizeof(ptrdiff_t);
        using delta_type = std::conditional_t<has_t_arithmetic, T, std::conditional_t<prefer_ptrdiff_arithmetic, ptrdiff_t, int>>;
    public:
        class iterator: public FlexibleRandomAccessIterator<iterator, const T> {
        public:
            using iterator_category = std::conditional_t<has_random_access, std::random_access_iterator_tag, std::bidirectional_iterator_tag>;
            iterator() = default;
            explicit iterator(T t): value_(t) {}
            const T& operator*() const noexcept { return value_; }
            iterator& operator++() { ++value_; return *this; }
            iterator& operator--() { --value_; return *this; }
            iterator& operator+=(ptrdiff_t b) { value_ += delta_type(b); return *this; }
            ptrdiff_t operator-(const iterator& b) { return ptrdiff_t(value_ - b.value_); }
            bool operator==(const iterator& b) const noexcept { return value_ == b.value_; }
        private:
            T value_;
        };
        iterator begin() const { return this->empty() ? iterator() : iterator(this->min_); }
        iterator end() const { return this->empty() ? iterator() : std::next(iterator(this->max_)); }
        size_t size() const;
    protected:
        void adjust_bounds();
    };

        template <typename T>
        size_t IntervalCategoryBase<T, IntervalCategory::integral>::size() const {
            if (this->empty())
                return 0;
            if (this->is_infinite())
                return npos;
            if constexpr (Meta::has_minus_operator<T> && std::is_constructible_v<size_t, T>) {
                return static_cast<size_t>(this->max_ - this->min_) + 1;
            } else {
                size_t n = 1;
                for (T t = this->min_; t != this->max_; ++t, ++n) {}
                return n;
            }
        }

        template <typename T>
        void IntervalCategoryBase<T, IntervalCategory::integral>::adjust_bounds() {
            if (this->left_ == IB::open) {
                ++this->min_;
                this->left_ = IB::closed;
            }
            if (this->right_ == IB::open) {
                --this->max_;
                this->right_ = IB::closed;
            }
            IntervalTypeBase<T>::adjust_bounds();
        }

    template <typename T>
    class IntervalCategoryBase<T, IntervalCategory::continuous>:
    public IntervalTypeBase<T> {
    public:
        T size() const { return this->max_ - this->min_; }
    };

    // Base class for interval arithmetic

    template <typename IntervalType, typename T, IntervalCategory Cat = interval_category<T>>
    class IntervalArithmeticBase {
    public:
        IntervalType operator+() const { auto& a = static_cast<const IntervalType&>(*this); return a; }
        IntervalType operator-() const { auto& a = static_cast<const IntervalType&>(*this); return negative(a); }
        IntervalType& operator+=(const IntervalType& b) { auto& a = static_cast<IntervalType&>(*this); a = add(a, b); return a; }
        IntervalType& operator+=(const T& b) { auto& a = static_cast<IntervalType&>(*this); a = add(a, b); return a; }
        IntervalType& operator-=(const IntervalType& b) { auto& a = static_cast<IntervalType&>(*this); a = subtract(a, b); return a; }
        IntervalType& operator-=(const T& b) { auto& a = static_cast<IntervalType&>(*this); a = subtract(a, b); return a; }
        IntervalType& operator*=(const IntervalType& b) { auto& a = static_cast<IntervalType&>(*this); a = multiply(a, b); return a; }
        IntervalType& operator*=(const T& b) { auto& a = static_cast<IntervalType&>(*this); a = multiply(a, b); return a; }
        IntervalType& operator/=(const IntervalType& b) { auto& a = static_cast<IntervalType&>(*this); a = divide(a, b); return a; }
        IntervalType& operator/=(const T& b) { auto& a = static_cast<IntervalType&>(*this); a = divide(a, b); return a; }
        friend IntervalType operator+(const IntervalType& a, const IntervalType& b) { return add(a, b); }
        friend IntervalType operator+(const IntervalType& a, const T& b) { return add(a, b); }
        friend IntervalType operator+(const T& a, const IntervalType& b) { return add(b, a); }
        friend IntervalType operator-(const IntervalType& a, const IntervalType& b) { return subtract(a, b); }
        friend IntervalType operator-(const IntervalType& a, const T& b) { return subtract(a, b); }
        friend IntervalType operator-(const T& a, const IntervalType& b) { return subtract(a, b); }
        friend IntervalType operator*(const IntervalType& a, const IntervalType& b) { return multiply(a, b); }
        friend IntervalType operator*(const IntervalType& a, const T& b) { return multiply(a, b); }
        friend IntervalType operator*(const T& a, const IntervalType& b) { return multiply(b, a); }
        friend IntervalType operator/(const IntervalType& a, const IntervalType& b) { return divide(a, b); }
        friend IntervalType operator/(const IntervalType& a, const T& b) { return divide(a, b); }
        friend IntervalType operator/(const T& a, const IntervalType& b) { return divide(a, b); }
    private:
        using IB = IntervalBound;
        static IntervalType negative(const IntervalType& a);
        static IntervalType add(const IntervalType& a, const IntervalType& b);
        static IntervalType subtract(const IntervalType& a, const IntervalType& b);
        static IntervalType multiply(const IntervalType& a, const IntervalType& b);
        static IntervalType divide(const IntervalType& a, const IntervalType& b);
    };

        template <typename IntervalType, typename T, IntervalCategory Cat>
        IntervalType IntervalArithmeticBase<IntervalType, T, Cat>::negative(const IntervalType& a) {
            return IntervalType(- a.max(), - a.min(), a.right(), a.left());
        }

        template <typename IntervalType, typename T, IntervalCategory Cat>
        IntervalType IntervalArithmeticBase<IntervalType, T, Cat>::add(const IntervalType& a, const IntervalType& b) {
            if (a.empty() || b.empty())
                return {};
            else if (a.is_universal() || b.is_universal())
                return IntervalType::all();
            IB l = std::max(a.left(), b.left());
            IB r = std::max(a.right(), b.right());
            T t = l == IB::unbound ? T() : a.min() + b.min();
            T u = r == IB::unbound ? T() : a.max() + b.max();
            return IntervalType(t, u, l, r);
        }

        template <typename IntervalType, typename T, IntervalCategory Cat>
        IntervalType IntervalArithmeticBase<IntervalType, T, Cat>::subtract(const IntervalType& a, const IntervalType& b) {
            if (a.empty() || b.empty())
                return {};
            else if (a.is_universal() || b.is_universal())
                return IntervalType::all();
            IB l = std::max(a.left(), b.right());
            IB r = std::max(a.right(), b.left());
            T t = l == IB::unbound ? T() : a.min() - b.max();
            T u = r == IB::unbound ? T() : a.max() - b.min();
            return IntervalType(t, u, l, r);
        }

        template <typename IntervalType, typename T, IntervalCategory Cat>
        IntervalType IntervalArithmeticBase<IntervalType, T, Cat>::multiply(const IntervalType& a, const IntervalType& b) {
            // if (a.empty() || b.empty())
            //     return {};
            // else if ((a.is_single() && a.min() == T()) || (b.is_single() && b.min() == T()))
            //     return T();
            // else if (a.is_universal() || b.is_universal())
            //     return IntervalType::all();
            // TODO
            (void)a;
            (void)b;
            return {};
        }

        template <typename IntervalType, typename T, IntervalCategory Cat>
        IntervalType IntervalArithmeticBase<IntervalType, T, Cat>::divide(const IntervalType& a, const IntervalType& b) {
            // TODO
            (void)a;
            (void)b;
            return {};
        }

    template <typename IntervalType, typename T>
    class IntervalArithmeticBase<IntervalType, T, IntervalCategory::ordered> {};

    // Interval class

    template <typename T> class IntervalSet;

    template <typename T>
    class Interval:
    public IntervalCategoryBase<T>,
    public IntervalArithmeticBase<Interval<T>, T>,
    public LessThanComparable<Interval<T>> {
    private:
        using IB = IntervalBound;
        using IC = IntervalCategory;
        using IM = IntervalMatch;
        using IO = IntervalOrder;
    public:
        using value_type = T;
        static constexpr auto category = interval_category<T>;
        Interval() = default;
        Interval(const T& t): Interval(t, t, IB::closed, IB::closed) {}
        Interval(const T& t, IB l, IB r): Interval(t, t, l, r) {}
        Interval(const T& min, const T& max, IB lr = IB::closed): Interval(min, max, lr, lr) {}
        Interval(const T& min, const T& max, IB l, IB r);
        Interval(const T& min, const T& max, Uview mode);
        explicit operator bool() const noexcept { return ! this->empty(); }
        bool operator()(const T& t) const { return contains(t); }
        bool contains(const T& t) const { return this->match(t) == IM::ok; }
        IntervalSet<T> inverse() const;
        IO order(const Interval& b) const;
        int compare(const Interval& b) const noexcept;
        bool includes(const Interval& b) const;      // True if b is a subset of this
        bool overlaps(const Interval& b) const;      // True if the intersection is not empty
        bool touches(const Interval& b) const;       // True if there is no gap between this and b
        Interval envelope(const Interval& b) const;  // Spanning interval, including any gap
        Interval set_intersection(const Interval& b) const;
        IntervalSet<T> set_union(const Interval& b) const;
        IntervalSet<T> set_difference(const Interval& b) const;
        IntervalSet<T> set_symmetric_difference(const Interval& b) const;
        bool parse(Uview view) noexcept;
        Ustring str() const;
        void swap(Interval& in) noexcept { this->do_swap(in); }
        static Interval all() noexcept { return Interval(T(), IB::unbound, IB::unbound); }
    };

        template <typename T>
        Interval<T>::Interval(const T& min, const T& max, IB l, IB r) {
            this->min_ = min;
            this->max_ = max;
            this->left_ = l;
            this->right_ = r;
            this->adjust_bounds();
        }

        template <typename T>
        Interval<T>::Interval(const T& min, const T& max, Uview mode) {
            this->min_ = min;
            this->max_ = max;
            std::tie(this->left_, this->right_) = this->parse_interval_mode(mode);
            this->adjust_bounds();
        }

        template <typename T>
        IntervalSet<T> Interval<T>::inverse() const {
            if (this->empty())
                return all();
            if (this->is_universal())
                return {};
            IntervalSet<T> set;
            if (this->is_left_bounded())
                set.insert({this->min(), IB::unbound, ~ this->left()});
            if (this->is_right_bounded())
                set.insert({this->max(), ~ this->right(), IB::unbound});
            return set;
        }

        template <typename T>
        IntervalOrder Interval<T>::order(const Interval& b) const {
            auto& a = *this;
            if (a.empty() && b.empty())  return IO::equal;
            if (a.empty())               return IO::b_only;
            if (b.empty())               return IO::a_only;
            auto [al, ar] = a.find_interval_bounds();
            auto [bl, br] = b.find_interval_bounds();
            if (ar < bl) {
                if (this->adjacent_intervals(ar, bl))  return IO::a_touches_b;
                else                                   return IO::a_below_b;
            } else if (br < al) {
                if (this->adjacent_intervals(br, al))  return IO::b_touches_a;
                else                                   return IO::b_below_a;
            } else if (al < bl) {
                if (ar < br)       return IO::a_overlaps_b;
                else if (br < ar)  return IO::a_encloses_b;
                else               return IO::a_extends_below_b;
            } else if (bl < al) {
                if (ar < br)       return IO::b_encloses_a;
                else if (br < ar)  return IO::b_overlaps_a;
                else               return IO::b_extends_below_a;
            } else {
                if (ar < br)       return IO::b_extends_above_a;
                else if (br < ar)  return IO::a_extends_above_b;
                else               return IO::equal;
            }
        }

        template <typename T>
        int Interval<T>::compare(const Interval& b) const noexcept {
            auto& a = *this;
            if (a.empty() || b.empty())
                return int(b.empty()) - int(a.empty());
            if (a.is_left_bounded() && b.is_left_bounded()) {
                if (a.min() > b.min())
                    return 1;
                else if (a.min() < b.min())
                    return -1;
                else if (a.is_left_closed() != b.is_left_closed())
                    return a.is_left_closed() ? -1 : 1;
            } else if (a.is_left_bounded() || b.is_left_bounded()) {
                return a.is_left_bounded() ? 1 : -1;
            }
            if (a.is_right_bounded() && b.is_right_bounded()) {
                if (a.max() < b.max())
                    return -1;
                else if (a.max() > b.max())
                    return 1;
                else if (a.is_right_closed() != b.is_right_closed())
                    return a.is_right_closed() ? 1 : -1;
            } else if (a.is_right_bounded() || b.is_right_bounded()) {
                return a.is_right_bounded() ? -1 : 1;
            }
            return 0;
        }

        template <typename T>
        bool Interval<T>::includes(const Interval& b) const {
            IO ord = order(b);
            if (ord == IO::equal)
                return ! this->empty();
            else
                return ord == IO::a_extends_above_b || ord == IO::a_extends_below_b || ord == IO::a_encloses_b;
        }

        template <typename T>
        bool Interval<T>::overlaps(const Interval& b) const {
            IO ord = order(b);
            if (ord == IO::equal)
                return ! this->empty();
            else if (ord == IO::a_only || ord == IO::b_only
                    || ord == IO::a_below_b || ord == IO::b_below_a
                    || ord == IO::a_touches_b || ord == IO::b_touches_a)
                return false;
            else
                return true;
        }

        template <typename T>
        bool Interval<T>::touches(const Interval& b) const {
            IO ord = order(b);
            if (ord == IO::equal)
                return ! this->empty();
            else if (ord == IO::a_only || ord == IO::b_only || ord == IO::a_below_b || ord == IO::b_below_a)
                return false;
            else
                return true;
        }

        template <typename T>
        Interval<T> Interval<T>::envelope(const Interval& b) const {
            auto& a = *this;
            switch (order(b)) {
                case IO::a_below_b:
                case IO::a_overlaps_b:
                case IO::a_touches_b:
                    return {a.min(), b.max(), a.left(), b.right()};
                case IO::b_below_a:
                case IO::b_overlaps_a:
                case IO::b_touches_a:
                    return {b.min(), a.max(), b.left(), a.right()};
                case IO::b_encloses_a:
                case IO::b_extends_above_a:
                case IO::b_extends_below_a:
                case IO::b_only:
                    return b;
                default:
                    return a;
            }
        }

        template <typename T>
        Interval<T> Interval<T>::set_intersection(const Interval& b) const {
            auto& a = *this;
            switch (order(b)) {
                case IO::b_overlaps_a:
                    return {a.min(), b.max(), a.left(), b.right()};
                case IO::a_overlaps_b:
                    return {b.min(), a.max(), b.left(), a.right()};
                case IO::b_encloses_a:
                case IO::b_extends_above_a:
                case IO::b_extends_below_a:
                case IO::equal:
                    return a;
                case IO::a_encloses_b:
                case IO::a_extends_above_b:
                case IO::a_extends_below_b:
                    return b;
                default:
                    return {};
            }
        }

        template <typename T>
        IntervalSet<T> Interval<T>::set_union(const Interval& b) const {
            auto& a = *this;
            if (a.empty() && b.empty())
                return {};
            switch (order(b)) {
                case IO::a_below_b:
                    return {a,b};
                case IO::a_overlaps_b:
                case IO::a_touches_b:
                    return {{a.min(), b.max(), a.left(), b.right()}};
                case IO::b_below_a:
                    return {b,a};
                case IO::b_overlaps_a:
                case IO::b_touches_a:
                    return {{b.min(), a.max(), b.left(), a.right()}};
                case IO::b_encloses_a:
                case IO::b_extends_above_a:
                case IO::b_extends_below_a:
                case IO::b_only:
                    return {b};
                default:
                    return {a};
            }
        }

        template <typename T>
        IntervalSet<T> Interval<T>::set_difference(const Interval& b) const {
            auto& a = *this;
            switch (order(b)) {
                case IO::a_extends_above_b:
                case IO::b_overlaps_a:
                    return {{b.max(), a.max(), ~ b.right(), a.right()}};
                case IO::a_extends_below_b:
                case IO::a_overlaps_b:
                    return {{a.min(), b.min(), a.left(), ~ b.left()}};
                case IO::a_encloses_b:
                    return {{a.min(), b.min(), a.left(), ~ b.left()}, {b.max(), a.max(), ~ b.right(), a.right()}};
                case IO::a_below_b:
                case IO::a_only:
                case IO::a_touches_b:
                case IO::b_below_a:
                case IO::b_touches_a:
                    return {a};
                default:
                    return {};
            }
        }

        template <typename T>
        IntervalSet<T> Interval<T>::set_symmetric_difference(const Interval& b) const {
            auto& a = *this;
            switch (order(b)) {
                case IO::b_only:             return {b};
                case IO::a_below_b:          return {a, b};
                case IO::a_touches_b:        return {{a.min(), b.max(), a.left(), b.right()}};
                case IO::a_overlaps_b:       return {{a.min(), b.min(), a.left(), ~ b.left()}, {a.max(), b.max(), ~ a.right(), b.right()}};
                case IO::a_extends_below_b:  return {{a.min(), b.min(), a.left(), ~ b.left()}};
                case IO::a_encloses_b:       return {{a.min(), b.min(), a.left(), ~ b.left()}, {b.max(), a.max(), ~ b.right(), a.right()}};
                case IO::b_extends_above_a:  return {{a.max(), b.max(), ~ a.right(), b.right()}};
                case IO::a_extends_above_b:  return {{b.max(), a.max(), ~ b.right(), a.right()}};
                case IO::b_encloses_a:       return {{b.min(), a.min(), b.left(), ~ a.left()}, {a.max(), b.max(), ~ a.right(), b.right()}};
                case IO::b_extends_below_a:  return {{b.min(), a.min(), b.left(), ~ a.left()}};
                case IO::b_overlaps_a:       return {{b.min(), a.min(), b.left(), ~ a.left()}, {b.max(), a.max(), ~ b.right(), a.right()}};
                case IO::b_touches_a:        return {{b.min(), a.max(), b.left(), a.right()}};
                case IO::b_below_a:          return {b, a};
                case IO::a_only:             return {a};
                default:                     return {};
            }
        }

        template <typename T>
        bool Interval<T>::parse(Uview view) noexcept {
            Uview sv_min, sv_max;
            IB left, right;
            if (! this->parse_interval_string(view, sv_min, sv_max, left, right))
                return false;
            bool left_bounded = left == IB::open || left == IB::closed;
            bool right_bounded = right == IB::open || right == IB::closed;
            T t_min{}, t_max{};
            if (left_bounded && ! from_str(sv_min, t_min))
                return false;
            if (right_bounded) {
                if (left_bounded && sv_min == sv_max)
                    t_max = t_min;
                else if (! from_str(sv_max, t_max))
                    return false;
            }
            *this = {t_min, t_max, left, right};
            return true;
        }

        template <typename T>
        Ustring Interval<T>::str() const {
            Ustring s;
            if (this->empty()) {
                s = "{}";
            } else if (this->is_universal()) {
                s = "*";
            } else if (this->is_single()) {
                s = to_str(this->min());
            } else if (! this->is_left_bounded()) {
                s += '<';
                if (this->is_right_closed())
                    s += '=';
                s += to_str(this->max());
            } else if (! this->is_right_bounded()) {
                s += '>';
                if (this->is_left_closed())
                    s += '=';
                s += to_str(this->min());
            } else {
                s += this->is_left_closed() ? '[' : '(';
                s += to_str(this->min());
                s += ',';
                s += to_str(this->max());
                s += this->is_right_closed() ? ']' : ')';
            }
            return s;
        }

    template <typename T> bool operator==(const Interval<T>& a, const Interval<T>& b) noexcept { return a.compare(b) == 0; }
    template <typename T> bool operator<(const Interval<T>& a, const Interval<T>& b) noexcept { return a.compare(b) < 0; }
    template <typename T> std::ostream& operator<<(std::ostream& out, const Interval<T>& in) { return out << in.str(); }
    template <typename T> void from_json(const json& j, Interval<T>& in) { in.convert_from_json(j); }
    template <typename T> void to_json(json& j, const Interval<T>& in) { j = in.convert_to_json(); }
    template <typename T> bool from_str(Uview view, Interval<T>& in) { return in.parse(view); }
    template <typename T> Ustring to_str(const Interval<T>& in) { return in.str(); }
    template <typename T> void swap(Interval<T>& a, Interval<T>& b) { a.swap(b); }
    template <typename T> Interval<T> make_interval(const T& t) { return Interval<T>(t); }
    template <typename T> Interval<T> make_interval(const T& t, IntervalBound l, IntervalBound r) { return Interval<T>(t, l, r); }
    template <typename T> Interval<T> make_interval(const T& min, const T& max, IntervalBound lr = IntervalBound::closed) { return Interval<T>(min, max, lr); }
    template <typename T> Interval<T> make_interval(const T& min, const T& max, IntervalBound l, IntervalBound r) { return Interval<T>(min, max, l, r); }
    template <typename T> Interval<T> make_interval(const T& min, const T& max, Uview mode) { return Interval<T>(min, max, mode); }

    template <typename T> Interval<T>
    ordered_interval(T a, T b, IntervalBound lr = IntervalBound::closed) {
        if (b < a)
            std::swap(a, b);
        return Interval<T>(a, b, lr);
    }

    template <typename T> Interval<T>
    ordered_interval(T a, T b, IntervalBound l, IntervalBound r) {
        if (b < a) {
            std::swap(a, b);
            std::swap(l, r);
        }
        return Interval<T>(a, b, l, r);
    }

    // Common base class for associative containers

    class IntervalAssociativeBase {
    protected:
        using views = std::vector<Uview>;
        using view_pairs = std::vector<std::pair<Uview, Uview>>;
        static void parse_interval_set_string(Uview src, views& dst);
        static bool parse_interval_map_string(Uview src, view_pairs& dst);
    };

    // Interval set

    template <typename T>
    class IntervalSet:
    public IntervalAssociativeBase,
    public LessThanComparable<IntervalSet<T>> {
    public:
        using value_type = T;
        using interval_type = Interval<T>;
    private:
        using container_type = std::set<interval_type>;
        using IB = IntervalBound;
        using IM = IntervalMatch;
        using IO = IntervalOrder;
    public:
        using iterator = typename container_type::const_iterator;
        static constexpr auto category = interval_category<T>;
        IntervalSet() = default;
        IntervalSet(const T& t): con_{{t}} {}
        IntervalSet(const interval_type& in): con_{in} {}
        IntervalSet(std::initializer_list<interval_type> list): con_(list) {}
        bool operator()(const T& t) const { return contains(t); }
        IntervalSet operator~() const { return inverse(); }
        IntervalSet& operator&=(const IntervalSet<T>& b) { *this = set_intersection(b); return *this; }
        IntervalSet& operator|=(const IntervalSet<T>& b) { *this = set_union(b); return *this; }
        IntervalSet& operator-=(const IntervalSet<T>& b) { *this = set_difference(b); return *this; }
        IntervalSet& operator^=(const IntervalSet<T>& b) { *this = set_symmetric_difference(b); return *this; }
        auto begin() const noexcept { return con_.begin(); }
        auto end() const noexcept { return con_.end(); }
        bool empty() const noexcept { return con_.empty(); }
        size_t size() const noexcept { return con_.size(); }
        bool contains(const T& t) const;
        void clear() noexcept { con_.clear(); }
        void insert(const interval_type& in);
        void erase(const interval_type& in);
        IntervalSet inverse() const;
        IntervalSet set_intersection(const IntervalSet& b) const;
        IntervalSet set_union(const IntervalSet& b) const;
        IntervalSet set_difference(const IntervalSet& b) const;
        IntervalSet set_symmetric_difference(const IntervalSet& b) const;
        size_t hash() const noexcept { return hash_range(con_); }
        bool parse(Uview view) noexcept;
        Ustring str() const { return format_list(con_, "{", ",", "}"); }
        void swap(IntervalSet& set) noexcept { con_.swap(set.con_); }
    private:
        container_type con_;
    };

        template <typename T>
        bool IntervalSet<T>::contains(const T& t) const {
            interval_type in(t);
            auto i = con_.lower_bound(in);
            if (i != con_.begin())
                --i;
            for (; i != con_.end(); ++i) {
                auto m = i->match(t);
                if (m == IM::ok)
                    return true;
                else if (m == IM::low)
                    return false;
            }
            return false;
        }

        template <typename T>
        void IntervalSet<T>::insert(const interval_type& in) {
            if (in.empty())
                return;
            auto i = con_.lower_bound(in);
            if (i != con_.begin())
                --i;
            auto add = in;
            while (i != con_.end()) {
                auto ord = in.order(*i);
                if (ord <= IO::a_below_b)
                    break;
                auto j = i++;
                if (ord <= IO::b_touches_a) {
                    add = add.envelope(*j);
                    con_.erase(j);
                }
            }
            con_.insert(add);
        }

        template <typename T>
        void IntervalSet<T>::erase(const interval_type& in) {
            if (empty() || in.empty())
                return;
            auto i = con_.lower_bound(in);
            if (i != con_.begin())
                --i;
            IntervalSet temp;
            std::vector<interval_type> vec;
            while (i != con_.end()) {
                auto ord = in.order(*i);
                if (ord <= IO::a_touches_b)
                    break;
                auto j = i++;
                if (ord <= IO::b_overlaps_a) {
                    temp = j->set_difference(in);
                    std::copy(temp.begin(), temp.end(), append(vec));
                    con_.erase(j);
                }
            }
            con_.insert(vec.begin(), vec.end());
        }

        template <typename T>
        IntervalSet<T> IntervalSet<T>::inverse() const {
            if (empty())
                return interval_type::all();
            IntervalSet result;
            auto i = con_.begin();
            if (i->is_left_bounded())
                result.con_.insert({{}, i->min(), IB::unbound, ~ i->left()});
            for (auto j = std::next(i), end = con_.end(); j != end; i = j++)
                result.con_.insert({i->max(), j->min(), ~ i->right(), ~ j->left()});
            if (i->is_right_bounded())
                result.con_.insert({i->max(), {}, ~ i->right(), IB::unbound});
            return result;
        }

        template <typename T>
        IntervalSet<T> IntervalSet<T>::set_intersection(const IntervalSet& b) const {
            return inverse().set_union(b.inverse()).inverse();
        }

        template <typename T>
        IntervalSet<T> IntervalSet<T>::set_union(const IntervalSet& b) const {
            auto result = *this;
            for (auto& in: b)
                result.insert(in);
            return result;
        }

        template <typename T>
        IntervalSet<T> IntervalSet<T>::set_difference(const IntervalSet& b) const {
            auto result = *this;
            for (auto& in: b)
                result.erase(in);
            return result;
        }

        template <typename T>
        IntervalSet<T> IntervalSet<T>::set_symmetric_difference(const IntervalSet& b) const {
            auto set1 = set_difference(b);
            auto set2 = b.set_difference(*this);
            return set1.set_union(set2);
        }

        template <typename T>
        bool IntervalSet<T>::parse(Uview view) noexcept {
            interval_type in;
            IntervalSet set;
            if (in.parse(view)) {
                set.insert(in);
            } else {
                views fields;
                parse_interval_set_string(view, fields);
                for (auto& field: fields) {
                    if (! in.parse(field))
                        return false;
                    set.insert(in);
                }
            }
            con_ = std::move(set.con_);
            return true;
        }

    template <typename T> IntervalSet<T> operator&(const IntervalSet<T>& a, const IntervalSet<T>& b) { return a.set_intersection(b); }
    template <typename T> IntervalSet<T> operator&(const IntervalSet<T>& a, const Interval<T>& b) { return a.set_intersection(b); }
    template <typename T> IntervalSet<T> operator&(const Interval<T>& a, const IntervalSet<T>& b) { return a.set_intersection(b); }
    template <typename T> IntervalSet<T> operator&(const IntervalSet<T>& a, const T& b) { return IntervalSet<T>(a).set_intersection(b); }
    template <typename T> IntervalSet<T> operator&(const T& a, const IntervalSet<T>& b) { return IntervalSet<T>(a).set_intersection(b); }
    template <typename T> IntervalSet<T> operator|(const IntervalSet<T>& a, const IntervalSet<T>& b) { return a.set_union(b); }
    template <typename T> IntervalSet<T> operator|(const IntervalSet<T>& a, const Interval<T>& b) { return a.set_union(b); }
    template <typename T> IntervalSet<T> operator|(const Interval<T>& a, const IntervalSet<T>& b) { return a.set_union(b); }
    template <typename T> IntervalSet<T> operator|(const IntervalSet<T>& a, const T& b) { return IntervalSet<T>(a).set_union(b); }
    template <typename T> IntervalSet<T> operator|(const T& a, const IntervalSet<T>& b) { return IntervalSet<T>(a).set_union(b); }
    template <typename T> IntervalSet<T> operator-(const IntervalSet<T>& a, const IntervalSet<T>& b) { return a.set_difference(b); }
    template <typename T> IntervalSet<T> operator-(const IntervalSet<T>& a, const Interval<T>& b) { return a.set_difference(b); }
    template <typename T> IntervalSet<T> operator-(const Interval<T>& a, const IntervalSet<T>& b) { return a.set_difference(b); }
    template <typename T> IntervalSet<T> operator-(const IntervalSet<T>& a, const T& b) { return IntervalSet<T>(a).set_difference(b); }
    template <typename T> IntervalSet<T> operator-(const T& a, const IntervalSet<T>& b) { return IntervalSet<T>(a).set_difference(b); }
    template <typename T> IntervalSet<T> operator^(const IntervalSet<T>& a, const IntervalSet<T>& b) { return a.set_symmetric_difference(b); }
    template <typename T> IntervalSet<T> operator^(const IntervalSet<T>& a, const Interval<T>& b) { return a.set_symmetric_difference(b); }
    template <typename T> IntervalSet<T> operator^(const Interval<T>& a, const IntervalSet<T>& b) { return a.set_symmetric_difference(b); }
    template <typename T> IntervalSet<T> operator^(const IntervalSet<T>& a, const T& b) { return IntervalSet<T>(a).set_symmetric_difference(b); }
    template <typename T> IntervalSet<T> operator^(const T& a, const IntervalSet<T>& b) { return IntervalSet<T>(a).set_symmetric_difference(b); }
    template <typename T> bool operator==(const IntervalSet<T>& a, const IntervalSet<T>& b) noexcept
        { return std::equal(a.begin(), a.end(), b.begin(), b.end()); }
    template <typename T> bool operator<(const IntervalSet<T>& a, const IntervalSet<T>& b) noexcept
        { return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); }
    template <typename T> std::ostream& operator<<(std::ostream& out, const IntervalSet<T>& set) { return out << set.str(); }
    template <typename T> bool from_str(Uview view, IntervalSet<T>& set) { return set.parse(view); }
    template <typename T> Ustring to_str(const IntervalSet<T>& set) { return set.str(); }
    template <typename T> void swap(IntervalSet<T>& a, IntervalSet<T>& b) { a.swap(b); }

    // Interval map

    template <typename K, typename T>
    class IntervalMap:
    public IntervalAssociativeBase,
    public LessThanComparable<IntervalMap<K, T>> {
    public:
        using key_type = K;
        using mapped_type = T;
        using interval_type = Interval<K>;
    private:
        using container_type = std::map<interval_type, T>;
        using IM = IntervalMatch;
        using IO = IntervalOrder;
    public:
        using iterator = typename container_type::const_iterator;
        using value_type = typename container_type::value_type;
        static constexpr auto category = interval_category<K>;
        IntervalMap() = default;
        explicit IntervalMap(const T& defval): con_(), def_(defval) {}
        IntervalMap(std::initializer_list<value_type> list) { for (auto& v: list) insert(v); }
        const T& operator[](const K& key) const;
        auto begin() const noexcept { return con_.begin(); }
        auto end() const noexcept { return con_.end(); }
        bool empty() const noexcept { return con_.empty(); }
        size_t size() const noexcept { return con_.size(); }
        const T& default_value() const noexcept { return def_; }
        void default_value(const T& defval) { def_ = defval; }
        bool contains(const K& key) const { return do_find(key).second; }
        iterator find(const K& key) const;
        iterator lower_bound(const K& key) const { return do_find(key).first; }
        iterator upper_bound(const K& key) const;
        void clear() noexcept { con_.clear(); }
        void reset(const T& defval = {}) { def_ = defval; clear(); }
        void insert(const interval_type& in, const T& t);
        void insert(const value_type& v) { insert(v.first, v.second); }
        void erase(const interval_type& in);
        size_t hash() const noexcept;
        bool parse(Uview view) noexcept;
        Ustring str() const { return format_map(con_); }
        void swap(IntervalMap& map) noexcept { con_.swap(map.con_); std::swap(def_, map.def_); }
    private:
        container_type con_;
        T def_ = T();
        std::pair<iterator, bool> do_find(const K& key) const;
    };

        template <typename K, typename T>
        const T& IntervalMap<K, T>::operator[](const K& key) const {
            auto [it,ok] = do_find(key);
            return ok ? it->second : def_;
        }

        template <typename K, typename T>
        typename IntervalMap<K, T>::iterator IntervalMap<K, T>::find(const K& key) const {
            auto [it,ok] = do_find(key);
            return ok ? it : end();
        }

        template <typename K, typename T>
        typename IntervalMap<K, T>::iterator IntervalMap<K, T>::upper_bound(const K& key) const {
            auto [it,ok] = do_find(key);
            if (ok)
                ++it;
            return it;
        }

        template <typename K, typename T>
        void IntervalMap<K, T>::insert(const interval_type& in, const T& t) {
            if (in.empty())
                return;
            auto key = in;
            auto i = con_.upper_bound(key);
            if (i != con_.begin())
                --i;
            std::vector<value_type> add;
            std::vector<iterator> del;
            for (; i != con_.end(); ++i) {
                auto ord = key.order(i->first);
                if (ord <= IO::a_below_b) {
                    break;
                } else if (ord <= IO::b_touches_a && i->second == t) {
                    key = key.envelope(i->first);
                    del.push_back(i);
                } else if (ord == IO::b_touches_a) {
                    break;
                } else if (ord <= IO::b_overlaps_a) {
                    auto diff = i->first.set_difference(in);
                    for (auto& d: diff)
                        add.push_back({d, i->second});
                    del.push_back(i);
                }
            }
            for (auto& d: del)
                con_.erase(d);
            for (auto& a: add)
                con_.insert(a);
            con_.insert({key, t});
        }

        template <typename K, typename T>
        void IntervalMap<K, T>::erase(const interval_type& in) {
            if (empty() || in.empty())
                return;
            auto i = con_.lower_bound(in);
            if (i != con_.begin())
                --i;
            std::vector<value_type> vec;
            while (i != con_.end()) {
                auto ord = in.order(i->first);
                if (ord <= IO::a_touches_b)
                    break;
                auto j = i++;
                if (ord <= IO::b_overlaps_a) {
                    auto temp = j->first.set_difference(in);
                    for (auto& t: temp)
                        vec.push_back({t, j->second});
                    con_.erase(j);
                }
            }
            con_.insert(vec.begin(), vec.end());
        }

        template <typename K, typename T>
        size_t IntervalMap<K, T>::hash() const noexcept {
            size_t h = std::hash<T>()(def_);
            for (auto& [k,t]: con_)
                hash_combine(h, k, t);
            return h;
        }

        template <typename K, typename T>
        bool IntervalMap<K, T>::parse(Uview view) noexcept {
            view_pairs pairs;
            if (! parse_interval_map_string(view, pairs))
                return false;
            interval_type k;
            T t = {};
            IntervalMap map;
            for (auto& [first,second]: pairs) {
                if (! k.parse(first) || ! from_str(second, t))
                    return false;
                map.insert(k, t);
            }
            con_ = std::move(map.con_);
            return true;
        }

        template <typename K, typename T>
        std::pair<typename IntervalMap<K, T>::iterator, bool> IntervalMap<K, T>::do_find(const K& key) const {
            if (empty())
                return {end(), false};
            interval_type in(key);
            auto it = con_.lower_bound(in);
            if (it != con_.begin())
                --it;
            for (; it != con_.end(); ++it) {
                auto m = it->first.match(key);
                if (m != IM::high)
                    return {it, m == IM::ok};
            }
            return {end(), false};
        }

    template <typename T, typename K> bool operator==(const IntervalMap<K, T>& a, const IntervalMap<K, T>& b) noexcept
        { return std::equal(a.begin(), a.end(), b.begin(), b.end()); }
    template <typename T, typename K> bool operator<(const IntervalMap<K, T>& a, const IntervalMap<K, T>& b) noexcept
        { return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); }
    template <typename T, typename K> std::ostream& operator<<(std::ostream& out, const IntervalMap<K, T>& set) { return out << set.str(); }
    template <typename T, typename K> bool from_str(Uview view, IntervalMap<K, T>& set) { return set.parse(view); }
    template <typename T, typename K> Ustring to_str(const IntervalMap<K, T>& set) { return set.str(); }
    template <typename T, typename K> void swap(IntervalMap<K, T>& a, IntervalMap<K, T>& b) { a.swap(b); }

}

namespace std {

    template <typename T>
    class hash<RS::Interval<T>> {
    public:
        size_t operator()(const RS::Interval<T>& in) const noexcept { return in.hash(); }
    };

    template <typename T>
    class hash<RS::IntervalSet<T>> {
    public:
        size_t operator()(const RS::IntervalSet<T>& set) const noexcept { return set.hash(); }
    };

    template <typename K, typename T>
    class hash<RS::IntervalMap<K, T>> {
    public:
        size_t operator()(const RS::IntervalMap<K, T>& map) const noexcept { return map.hash(); }
    };

}

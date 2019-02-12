#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <deque>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    template <typename T>
    class AutoVector:
    public LessThanComparable<AutoVector<T>> {
    public:
        using index_type = size_t;
        using value_type = T;
        class const_iterator: public RandomAccessIterator<const_iterator, const T> {
        public:
            const_iterator() = default;
            const T& operator*() const noexcept { return obj_->get(pos_); }
            const_iterator& operator+=(ptrdiff_t rhs) noexcept { pos_ += rhs; return *this; }
            ptrdiff_t operator-(const const_iterator& rhs) const noexcept { return pos_ - rhs.pos_; }
        private:
            friend class AutoVector;
            const AutoVector* obj_ = nullptr;
            ptrdiff_t pos_ = npos;
            const_iterator(const AutoVector* obj, size_t pos) noexcept: obj_(obj), pos_(pos) {}
        };
        class iterator: public RandomAccessIterator<iterator, T> {
        public:
            iterator() = default;
            T& operator*() const noexcept { return obj_->ref(pos_); }
            iterator& operator+=(ptrdiff_t rhs) noexcept { pos_ += rhs; return *this; }
            ptrdiff_t operator-(const iterator& rhs) const noexcept { return pos_ - rhs.pos_; }
            operator const_iterator() const noexcept { return const_iterator(obj_, pos_); }
        private:
            friend class AutoVector;
            AutoVector* obj_ = nullptr;
            ptrdiff_t pos_ = npos;
            iterator(AutoVector* obj, size_t pos) noexcept: obj_(obj), pos_(pos) {}
        };
        AutoVector(): vec_(), def_() {}
        explicit AutoVector(const T& def): vec_(), def_(def) {}
        explicit AutoVector(size_t n, const T& t, const T& def = {}): vec_(n, t), def_(def) {}
        template <typename InIter> AutoVector(InIter i1, InIter i2, const T& def = {}): vec_(i1, i2), def_(def) {}
        AutoVector(std::initializer_list<T> list): vec_(list), def_() {}
        AutoVector& operator=(std::initializer_list<T> list) { vec_ = list; def_ = {}; return *this; }
        const T& get(size_t i) const noexcept { return i < vec_.size() ? vec_[i] : def_; }
        T& ref(size_t i);
        const T& operator[](size_t i) const noexcept { return get(i); }
        T& operator[](size_t i) noexcept { return ref(i); }
        iterator begin() noexcept { return iterator(this, 0); }
        const_iterator begin() const noexcept { return const_iterator(this, 0); }
        const_iterator cbegin() const noexcept { return const_iterator(this, 0); }
        iterator end() noexcept { return iterator(this, vec_.size()); }
        const_iterator end() const noexcept { return const_iterator(this, vec_.size()); }
        const_iterator cend() const noexcept { return const_iterator(this, vec_.size()); }
        bool empty() const noexcept { return vec_.empty(); }
        size_t size() const noexcept { return vec_.size(); }
        size_t capacity() const noexcept { return vec_.capacity(); }
        void clear() noexcept { vec_.clear(); }
        void clear(const T& def) { def_ = def; vec_.clear(); }
        void reserve(size_t n) { vec_.reserve(n); }
        void shrink_to_fit() noexcept { trim(); vec_.shrink_to_fit(); }
        void trim() noexcept;
        const T& get_default() const noexcept { return def_; }
        void set_default(const T& def) { def_ = def; }
        iterator insert(const_iterator i, const T& t);
        iterator insert(const_iterator i, size_t n, const T& t);
        template <typename InIter> iterator insert(const_iterator i, InIter j1, InIter j2);
        iterator insert(const_iterator i, std::initializer_list<T> list);
        template <typename... Args> iterator emplace(const_iterator i, Args&&... args);
        iterator erase(const_iterator i) noexcept;
        iterator erase(const_iterator i1, const_iterator i2) noexcept;
        void swap(AutoVector& obj) noexcept { vec_.swap(obj.vec_); std::swap(def_, obj.def_); }
        friend void swap(AutoVector& lhs, AutoVector& rhs) noexcept { lhs.swap(rhs); }
        bool operator==(const AutoVector& rhs) const noexcept;
        bool operator<(const AutoVector& rhs) const noexcept;
        friend std::ostream& operator<<(std::ostream& out, const AutoVector& obj) { return out << to_str(obj); }
    private:
        std::vector<T> vec_;
        T def_;
    };

        template <typename T>
        T& AutoVector<T>::ref(size_t i) {
            if (i >= vec_.size())
                vec_.resize(i + 1, def_);
            return vec_[i];
        }

        template <typename T>
        void AutoVector<T>::trim() noexcept {
            size_t n = size();
            while (n > 0 && vec_[n - 1] == def_)
                --n;
            vec_.resize(n);
        }

        template <typename T>
        typename AutoVector<T>::iterator AutoVector<T>::insert(const_iterator i, const T& t) {
            if (size_t(i.pos_) >= vec_.size()) {
                vec_.reserve(i.pos_ + 1);
                vec_.resize(i.pos_, def_);
            }
            vec_.insert(vec_.begin() + i.pos_, t);
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoVector<T>::iterator AutoVector<T>::insert(const_iterator i, size_t n, const T& t) {
            if (size_t(i.pos_) >= vec_.size()) {
                vec_.reserve(i.pos_ + n);
                vec_.resize(i.pos_, def_);
            }
            vec_.insert(vec_.begin() + i.pos_, n, t);
            return iterator(this, i.pos_);
        }

        template <typename T>
        template <typename InIter>
        typename AutoVector<T>::iterator AutoVector<T>::insert(const_iterator i, InIter j1, InIter j2) {
            if constexpr (std::is_same_v<typename std::iterator_traits<InIter>::iterator_category, std::random_access_iterator_tag>) {
                auto n = size_t(std::distance(j1, j2));
                if (size_t(i.pos_) >= vec_.size()) {
                    vec_.reserve(i.pos_ + n);
                    vec_.resize(i.pos_, def_);
                }
                vec_.insert(vec_.begin() + i.pos_, j1, j2);
            } else {
                for (auto k = i; j1 != j2;)
                    insert(k++, *j1++);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoVector<T>::iterator AutoVector<T>::insert(const_iterator i, std::initializer_list<T> list) {
            size_t n = list.size();
            if (size_t(i.pos_) >= vec_.size()) {
                vec_.reserve(i.pos_ + n);
                vec_.resize(i.pos_, def_);
            }
            vec_.insert(vec_.begin() + i.pos_, list);
            return iterator(this, i.pos_);
        }

        template <typename T>
        template <typename... Args>
        typename AutoVector<T>::iterator AutoVector<T>::emplace(const_iterator i, Args&&... args) {
            if (size_t(i.pos_) >= vec_.size()) {
                vec_.reserve(i.pos_ + 1);
                vec_.resize(i.pos_, def_);
            }
            vec_.emplace(vec_.begin() + i.pos_, std::forward<Args>(args)...);
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoVector<T>::iterator AutoVector<T>::erase(const_iterator i) noexcept {
            if (size_t(i.pos_) < vec_.size())
                vec_.erase(vec_.begin() + i.pos_);
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoVector<T>::iterator AutoVector<T>::erase(const_iterator i1, const_iterator i2) noexcept {
            if (size_t(i1.pos_) < vec_.size())
                vec_.erase(vec_.begin() + i1.pos_, vec_.begin() + std::min(i2.pos_, ptrdiff_t(vec_.size())));
            return iterator(this, i1.pos_);
        }

        template <typename T>
        bool AutoVector<T>::operator==(const AutoVector& rhs) const noexcept {
            if (def_ != rhs.def_)
                return false;
            size_t n1 = vec_.size(), n2 = rhs.vec_.size(), common = std::min(n1, n2);
            for (size_t i = 0; i < common; ++i)
                if (vec_[i] != rhs.vec_[i])
                    return false;
            if (n1 < n2) {
                for (size_t i = common; i < n2; ++i)
                    if (def_ != rhs.vec_[i])
                        return false;
            } else {
                for (size_t i = common; i < n1; ++i)
                    if (vec_[i] != rhs.def_)
                        return false;
            }
            return true;
        }

        template <typename T>
        bool AutoVector<T>::operator<(const AutoVector& rhs) const noexcept {
            size_t n1 = vec_.size(), n2 = rhs.vec_.size(), common = std::min(n1, n2);
            for (size_t i = 0; i < common; ++i)
                if (vec_[i] != rhs.vec_[i])
                    return vec_[i] < rhs.vec_[i];
            if (n1 < n2) {
                for (size_t i = common; i < n2; ++i)
                    if (def_ != rhs.vec_[i])
                        return def_ < rhs.vec_[i];
            } else {
                for (size_t i = common; i < n1; ++i)
                    if (vec_[i] != rhs.def_)
                        return vec_[i] < rhs.def_;
            }
            return def_ < rhs.def_;
        }

    template <typename T>
    class AutoDeque:
    public LessThanComparable<AutoDeque<T>> {
    public:
        using index_type = ptrdiff_t;
        using value_type = T;
        class const_iterator: public RandomAccessIterator<const_iterator, const T> {
        public:
            const_iterator() = default;
            const T& operator*() const noexcept { return obj_->get(pos_); }
            const_iterator& operator+=(ptrdiff_t rhs) noexcept { pos_ += rhs; return *this; }
            ptrdiff_t operator-(const const_iterator& rhs) const noexcept { return pos_ - rhs.pos_; }
        private:
            friend class AutoDeque;
            const AutoDeque* obj_ = nullptr;
            ptrdiff_t pos_ = npos;
            const_iterator(const AutoDeque* obj, ptrdiff_t pos) noexcept: obj_(obj), pos_(pos) {}
        };
        class iterator: public RandomAccessIterator<iterator, T> {
        public:
            iterator() = default;
            T& operator*() const noexcept { return obj_->ref(pos_); }
            iterator& operator+=(ptrdiff_t rhs) noexcept { pos_ += rhs; return *this; }
            ptrdiff_t operator-(const iterator& rhs) const noexcept { return pos_ - rhs.pos_; }
            operator const_iterator() const noexcept { return const_iterator(obj_, pos_); }
        private:
            friend class AutoDeque;
            AutoDeque* obj_ = nullptr;
            ptrdiff_t pos_ = npos;
            iterator(AutoDeque* obj, ptrdiff_t pos) noexcept: obj_(obj), pos_(pos) {}
        };
        AutoDeque(): deq_(), def_(), ofs_(0) {}
        explicit AutoDeque(const T& def): deq_(), def_(def), ofs_(0) {}
        explicit AutoDeque(size_t n, const T& t, const T& def = {}): deq_(n, t), def_(def), ofs_(0) {}
        template <typename InIter> AutoDeque(InIter i1, InIter i2, const T& def = {}): deq_(i1, i2), def_(def), ofs_(0) {}
        AutoDeque(std::initializer_list<T> list): deq_(list), def_(), ofs_(0) {}
        AutoDeque& operator=(std::initializer_list<T> list) { deq_ = list; def_ = {}; ofs_ = 0; return *this; }
        const T& get(ptrdiff_t i) const noexcept;
        T& ref(ptrdiff_t i);
        const T& operator[](ptrdiff_t i) const noexcept { return get(i); }
        T& operator[](ptrdiff_t i) noexcept { return ref(i); }
        iterator begin() noexcept { return iterator(this, ofs_); }
        const_iterator begin() const noexcept { return const_iterator(this, ofs_); }
        const_iterator cbegin() const noexcept { return const_iterator(this, ofs_); }
        iterator end() noexcept { return iterator(this, ofs_ + ptrdiff_t(deq_.size())); }
        const_iterator end() const noexcept { return const_iterator(this, ofs_ + ptrdiff_t(deq_.size())); }
        const_iterator cend() const noexcept { return const_iterator(this, ofs_ + ptrdiff_t(deq_.size())); }
        bool empty() const noexcept { return deq_.empty(); }
        size_t size() const noexcept { return deq_.size(); }
        ptrdiff_t min_index() const noexcept { return ofs_; }
        ptrdiff_t max_index() const noexcept { return ptrdiff_t(deq_.size()) + ofs_ - 1; }
        void clear() noexcept { deq_.clear(); ofs_ = 0; }
        void clear(const T& def) { def_ = def; deq_.clear(); ofs_ = 0; }
        void realign() noexcept { trim(); ofs_ = 0; }
        void trim() noexcept;
        const T& get_default() const noexcept { return def_; }
        void set_default(const T& def) { def_ = def; }
        iterator insert(const_iterator i, const T& t);
        iterator insert(const_iterator i, size_t n, const T& t);
        template <typename InIter> iterator insert(const_iterator i, InIter j1, InIter j2);
        iterator insert(const_iterator i, std::initializer_list<T> list);
        template <typename... Args> iterator emplace(const_iterator i, Args&&... args);
        iterator erase(const_iterator i) noexcept;
        iterator erase(const_iterator i1, const_iterator i2) noexcept;
        void swap(AutoDeque& obj) noexcept;
        friend void swap(AutoDeque& lhs, AutoDeque& rhs) noexcept { lhs.swap(rhs); }
        bool operator==(const AutoDeque& rhs) const noexcept;
        bool operator<(const AutoDeque& rhs) const noexcept;
        friend std::ostream& operator<<(std::ostream& out, const AutoDeque& obj) { return out << to_str(obj); }
    private:
        std::deque<T> deq_;
        T def_;
        ptrdiff_t ofs_;
    };

        template <typename T>
        const T& AutoDeque<T>::get(ptrdiff_t i) const noexcept {
            auto x = i - ofs_;
            if (x >= 0 && x < ptrdiff_t(deq_.size()))
                return deq_[x];
            else
                return def_;
        }

        template <typename T>
        T& AutoDeque<T>::ref(ptrdiff_t i) {
            auto x = i - ofs_;
            if (x < 0) {
                deq_.insert(deq_.begin(), - x, def_);
                ofs_ += x;
                x = 0;
            } else if (x >= ptrdiff_t(deq_.size())) {
                deq_.resize(x + 1, def_);
            }
            return deq_[x];
        }

        template <typename T>
        void AutoDeque<T>::trim() noexcept {
            auto b = deq_.begin(), e = deq_.end(), i = b;
            while (i != e && *i == def_)
                ++i;
            if (i == e)
                return;
            auto j = e - 1;
            while (*j == def_)
                --j;
            ++j;
            ofs_ += i - b;
            deq_.erase(b, i);
            deq_.erase(j, e);
        }

        template <typename T>
        typename AutoDeque<T>::iterator AutoDeque<T>::insert(const_iterator i, const T& t) {
            if (i.pos_ < ofs_) {
                deq_.insert(deq_.begin(), ofs_ - i.pos_ - 1, def_);
                deq_.push_front(t);
                ofs_ = i.pos_;
            } else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size())) {
                deq_.insert(deq_.begin() + (i.pos_ - ofs_), t);
            } else {
                deq_.resize(i.pos_ - ofs_, def_);
                deq_.push_back(t);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoDeque<T>::iterator AutoDeque<T>::insert(const_iterator i, size_t n, const T& t) {
            if (i.pos_ < ofs_) {
                deq_.insert(deq_.begin(), ofs_ - i.pos_ - 1, def_);
                deq_.insert(deq_.begin(), n, t);
                ofs_ = i.pos_;
            } else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size())) {
                deq_.insert(deq_.begin() + (i.pos_ - ofs_), n, t);
            } else {
                deq_.resize(i.pos_ - ofs_, def_);
                deq_.insert(deq_.end(), n, t);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        template <typename InIter>
        typename AutoDeque<T>::iterator AutoDeque<T>::insert(const_iterator i, InIter j1, InIter j2) {
            if (i.pos_ < ofs_) {
                deq_.insert(deq_.begin(), ofs_ - i.pos_ - 1, def_);
                deq_.insert(deq_.begin(), j1, j2);
                ofs_ = i.pos_;
            } else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size())) {
                deq_.insert(deq_.begin() + (i.pos_ - ofs_), j1, j2);
            } else {
                deq_.resize(i.pos_ - ofs_, def_);
                deq_.insert(deq_.end(), j1, j2);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoDeque<T>::iterator AutoDeque<T>::insert(const_iterator i, std::initializer_list<T> list) {
            if (i.pos_ < ofs_) {
                deq_.insert(deq_.begin(), ofs_ - i.pos_ - 1, def_);
                deq_.insert(deq_.begin(), list);
                ofs_ = i.pos_;
            } else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size())) {
                deq_.insert(deq_.begin() + (i.pos_ - ofs_), list);
            } else {
                deq_.resize(i.pos_ - ofs_, def_);
                deq_.insert(deq_.end(), list);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        template <typename... Args>
        typename AutoDeque<T>::iterator AutoDeque<T>::emplace(const_iterator i, Args&&... args) {
            if (i.pos_ < ofs_) {
                deq_.insert(deq_.begin(), ofs_ - i.pos_ - 1, def_);
                deq_.emplace_front(std::forward<Args>(args)...);
                ofs_ = i.pos_;
            } else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size())) {
                deq_.emplace(deq_.begin() + (i.pos_ - ofs_), std::forward<Args>(args)...);
            } else {
                deq_.resize(i.pos_ - ofs_, def_);
                deq_.emplace_back(std::forward<Args>(args)...);
            }
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoDeque<T>::iterator AutoDeque<T>::erase(const_iterator i) noexcept {
            if (i.pos_ < ofs_)
                --ofs_;
            else if (i.pos_ - ofs_ < ptrdiff_t(deq_.size()))
                deq_.erase(deq_.begin() + (i.pos_ - ofs_));
            if (deq_.empty())
                ofs_ = 0;
            return iterator(this, i.pos_);
        }

        template <typename T>
        typename AutoDeque<T>::iterator AutoDeque<T>::erase(const_iterator i1, const_iterator i2) noexcept {
            ptrdiff_t size = ptrdiff_t(deq_.size());
            ptrdiff_t x = i1.pos_ - ofs_;
            if (x < size) {
                ptrdiff_t y = std::min(i2.pos_ - ofs_, size);
                if (y >= 0) {
                    ptrdiff_t start = std::max(x, ptrdiff_t(0));
                    deq_.erase(deq_.begin() + start, deq_.begin() + y);
                    y = start;
                }
                if (x < 0)
                    ofs_ -= y - x;
                if (deq_.empty())
                    ofs_ = 0;
            }
            return iterator(this, i1.pos_);
        }

        template <typename T>
        void AutoDeque<T>::swap(AutoDeque& obj) noexcept {
            deq_.swap(obj.deq_);
            std::swap(def_, obj.def_);
            std::swap(ofs_, obj.ofs_);
        }

        template <typename T>
        bool AutoDeque<T>::operator==(const AutoDeque& rhs) const noexcept {
            if (def_ != rhs.def_) {
                return false;
            } else if (empty() && rhs.empty()) {
                return true;
            } else if (empty()) {
                return find_not(rhs.deq_.begin(), rhs.deq_.end(), def_) == rhs.deq_.end();
            } else if (rhs.empty()) {
                return find_not(deq_.begin(), deq_.end(), def_) == deq_.end();
            } else {
                ptrdiff_t min = std::min(min_index(), rhs.min_index());
                ptrdiff_t max = std::max(max_index(), rhs.max_index());
                for (ptrdiff_t i = min; i <= max; ++i)
                    if (get(i) != rhs.get(i))
                        return false;
                return true;
            }
        }

        template <typename T>
        bool AutoDeque<T>::operator<(const AutoDeque& rhs) const noexcept {
            if (def_ != rhs.def_) {
                return def_ < rhs.def_;
            } else if (empty() && rhs.empty()) {
                return false;
            } else if (empty()) {
                auto it = find_not(rhs.deq_.begin(), rhs.deq_.end(), def_);
                return it != rhs.deq_.end() && def_ < *it;
            } else if (rhs.empty()) {
                auto it = find_not(deq_.begin(), deq_.end(), def_);
                return it != deq_.end() && *it < def_;
            } else {
                ptrdiff_t min = std::min(min_index(), rhs.min_index());
                ptrdiff_t max = std::max(max_index(), rhs.max_index());
                for (ptrdiff_t i = min; i <= max; ++i)
                    if (get(i) != rhs.get(i))
                        return get(i) < rhs.get(i);
                return false;
            }
        }

}

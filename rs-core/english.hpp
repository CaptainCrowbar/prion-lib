#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"

namespace RS {

    // Case conversion functions

    namespace Detail {

        Ustring format_enum_helper(Ustring s, Uview flags);

    }

    Ustring extended_title_case(Uview str);
    void extended_title_case_in(Ustring& str);

    template <typename T>
    Ustring format_enum(T t, Uview flags = {}) {
        return Detail::format_enum_helper(to_str(t), Ustring(flags));
    }

    // List formatting functions

    namespace Detail {

        Ustring comma_list_helper(const Strings& list, Uview conj);

    }

    template <typename Range>
    Ustring comma_list(const Range& range, Uview conj = {}) {
        return Detail::comma_list_helper(to_strings(range), conj);
    }

    // Number formatting functions

    Ustring cardinal(size_t n, size_t threshold = npos);
    Ustring ordinal(size_t n, size_t threshold = npos);
    Ustring format_count(double x, int prec);
    Ustring number_of(size_t n, Uview name, Uview plural_name = {}, size_t threshold = 21);

    // Pluralization functions

    Ustring plural(Uview noun);

}

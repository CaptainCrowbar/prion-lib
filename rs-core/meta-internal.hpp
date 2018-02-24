#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

// Metaprogramming facilities used in rs-core/common; these are given a
// separate header to reduce build dependencies.

namespace RS::Meta {

    // Reflection tools

    // Walter E. Brown, N4502 Proposing Standard Library Support for the C++ Detection Idiom V2 (2015)
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf

    namespace MetaDetail {

        template <typename...> using VoidType = void;

        struct Nonesuch {
            Nonesuch() = delete;
            ~Nonesuch() = delete;
            Nonesuch(const Nonesuch&) = delete;
            Nonesuch(Nonesuch&&) = delete;
            void operator=(const Nonesuch&) = delete;
            void operator=(Nonesuch&&) = delete;
        };

        template <typename Default, typename, template <typename...> typename Archetype, typename... Args>
        struct Detector {
            using value_t = std::false_type;
            using type = Default;
        };

        template <typename Default, template <typename...> typename Archetype, typename... Args>
        struct Detector<Default, VoidType<Archetype<Args...>>, Archetype, Args...> {
            using value_t = std::true_type;
            using type = Archetype<Args...>;
        };

    }

    template <template <typename...> typename Archetype, typename... Args>
        using IsDetected = typename MetaDetail::Detector<MetaDetail::Nonesuch, void, Archetype, Args...>::value_t;
    template <template <typename...> typename Archetype, typename... Args>
        constexpr bool is_detected = IsDetected<Archetype, Args...>::value;
    template <template <typename...> typename Archetype, typename... Args>
        using DetectedType = typename MetaDetail::Detector<MetaDetail::Nonesuch, void, Archetype, Args...>::type;

    // Return nested type if detected, otherwise default
    template <typename Default, template <typename...> typename Archetype, typename... Args>
        using DetectedOr = typename MetaDetail::Detector<Default, void, Archetype, Args...>::type;

    // Detect only if it yields a specific return type
    template <typename Result, template <typename...> typename Archetype, typename... Args>
        using IsDetectedExact = std::is_same<Result, DetectedType<Archetype, Args...>>;
    template <typename Result, template <typename...> typename Archetype, typename... Args>
        constexpr bool is_detected_exact = IsDetectedExact<Result, Archetype, Args...>::value;

    // Detect only if it yields a return type convertible to the given type
    template <typename Result, template <typename...> typename Archetype, typename... Args>
        using IsDetectedConvertible = std::is_convertible<DetectedType<Archetype, Args...>, Result>;
    template <typename Result, template <typename...> typename Archetype, typename... Args>
        constexpr bool is_detected_convertible = IsDetectedConvertible<Result, Archetype, Args...>::value;

    // Type categories

    namespace MetaDetail {

        template <typename T> using IteratorCategory = typename std::iterator_traits<T>::iterator_category;

        template <typename T> using HasStdBeginArchetype = decltype(std::begin(std::declval<T>()));
        template <typename T> using HasAdlBeginArchetype = decltype(begin(std::declval<T>()));
        template <typename T> using HasStdEndArchetype = decltype(std::end(std::declval<T>()));
        template <typename T> using HasAdlEndArchetype = decltype(end(std::declval<T>()));

        template <typename T, bool Std = IsDetected<HasStdBeginArchetype, T>::value, bool Adl = IsDetected<HasAdlBeginArchetype, T>::value> struct RangeIteratorType { using type = void; };
        template <typename T, bool Adl> struct RangeIteratorType<T, true, Adl> { using type = decltype(std::begin(std::declval<T&>())); };
        template <typename T> struct RangeIteratorType<T, false, true> { using type = decltype(begin(std::declval<T&>())); };

    }

    template <typename T> using IsIterator = IsDetected<MetaDetail::IteratorCategory, T>;

    template <typename T> constexpr bool is_iterator = IsIterator<T>::value;

    namespace MetaDetail {

        template <typename T, bool Iter = Meta::is_iterator<T>> struct IteratorValueType { using type = void; };
        template <typename T> struct IteratorValueType<T, true> { using type = std::decay_t<decltype(*std::declval<T>())>; };

    }

    template <typename T> using IteratorValue = typename MetaDetail::IteratorValueType<T>::type;
    template <typename T> using RangeIterator = typename MetaDetail::RangeIteratorType<T>::type;
    template <typename T> using RangeValue = IteratorValue<RangeIterator<T>>;

}

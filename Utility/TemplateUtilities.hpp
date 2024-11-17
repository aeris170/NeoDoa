#pragma once

#include <string>
#include <concepts>

namespace concepts {

    template<typename T, typename ...U>
    concept IsAnyOf = (std::same_as<T, U> || ...);

    template<typename T>
    concept Copyable = requires(const T& t) {
        { T::Copy(t) } -> std::same_as<T>;
    };

    template<typename T>
    concept Serializable = requires {
        &T::Serialize;
        &T::Deserialize;
    };

    // https://en.cppreference.com/w/cpp/language/constraints
    // Declaration of the concept "Hashable", which is satisfied by any type 'T'
    // such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
    // compiles and its result is convertible to std::size_t
    template<typename T>
    concept Hashable = requires(T t) {
        { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
    };

    template<typename T>
    concept ConvertibleToString = requires (T t) {
        { std::to_string(t) } -> std::convertible_to<std::string>;
    };
}

namespace overloaded {
    template<class... Ts> struct lambda : Ts... { using Ts::operator()...; };
    template<class... Ts> lambda(Ts...)->lambda<Ts...>;
}

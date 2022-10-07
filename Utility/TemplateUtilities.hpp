#pragma once

#include <concepts>

namespace concepts {

    template<typename T, typename ...U>
    concept IsAnyOf = (std::same_as<T, U> || ...);

    template<typename T>
    concept Copyable = requires(const T & t) {
        { T::Copy(t) } -> std::same_as<T>;
    };

    template<typename T>
    concept Serializable = requires {
        &T::Serialize;
        &T::Deserialize;
    };
}

namespace overloaded {
    template<class... Ts> struct lambda : Ts... { using Ts::operator()...; };
    template<class... Ts> lambda(Ts...)->lambda<Ts...>;
}
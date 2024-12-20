#pragma once

#include <concepts>
#include <type_traits>

#include <Engine/Entity.hpp>

template<typename T>
concept ECSComponent = std::constructible_from<T, Entity> &&
    std::is_nothrow_copy_constructible_v<T> &&
    std::is_nothrow_copy_assignable_v<T> &&
    std::is_nothrow_move_constructible_v<T> &&
    std::is_nothrow_move_assignable_v<T>;
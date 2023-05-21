#pragma once

#include <entt/entt.hpp>

using Type = entt::type_info;

template<typename T>
Type GetType() noexcept {
    return entt::type_id<T>();
}
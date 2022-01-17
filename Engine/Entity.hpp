#pragma once

#include <string>

#include <entt.hpp>

using Entity = entt::entity;
constexpr Entity NULL_ENTT = entt::null;

namespace std {
	string to_string(const Entity entt);
}

template<typename T>
T EntityTo(const Entity entity) {
	return static_cast<T>(entity);
}

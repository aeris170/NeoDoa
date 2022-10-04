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
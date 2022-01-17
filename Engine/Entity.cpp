#include "Entity.hpp"

namespace std {
	string to_string(const Entity entt) {
		return to_string(EntityTo<uint32_t>(entt));
	}
}

#pragma once

#include <vector>
#include "TypedefsAndConstants.hpp"

struct Parent {

	std::vector<Entity> Children = {};
};

struct Child {

	Entity Of = NULL_ENTT;
};
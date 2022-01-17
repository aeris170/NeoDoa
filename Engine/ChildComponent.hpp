#pragma once

#include "Entity.hpp"

namespace tinyxml2 {
	class XMLPrinter;
	class XMLElement;
}

struct ChildComponent {
private:
	Entity entity{ NULL_ENTT };
	Entity parent{ NULL_ENTT };
	Entity _oldParent{ NULL_ENTT };

	ChildComponent(const Entity owner, const Entity parent) noexcept;

public:

	ChildComponent(const Entity owner) noexcept;

	Entity GetEntity() const;

	Entity GetParent() const;
	void SetParent(const Entity parent);

	friend void SerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& child);
	friend ChildComponent DeserializeChildComponent(tinyxml2::XMLElement* component, const Entity entity);

};
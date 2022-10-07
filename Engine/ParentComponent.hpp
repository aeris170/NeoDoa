#pragma once

#include <vector>

#include "Entity.hpp"

namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct ParentComponent {
private:
    Entity entity{ NULL_ENTT };
    std::vector<Entity> children{};

    ParentComponent(const Entity owner, std::vector<Entity>&& children) noexcept;

public:

    ParentComponent(const Entity owner) noexcept;

    Entity GetEntity() const;

    std::vector<Entity>& GetChildren();
    const std::vector<Entity>& GetChildren() const;

    friend void SerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& parent);
    friend ParentComponent DeserializeParentComponent(tinyxml2::XMLElement* component, const Entity entity);

};
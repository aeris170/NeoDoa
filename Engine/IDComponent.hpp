#pragma once

#include <string>

#include "Entity.hpp"

namespace tinyxml2 {
    class XMLPrinter;
    class XMLElement;
}

struct IDComponent {
private:

    Entity entity{ NULL_ENTT };
    std::string tag{ "???" };

public:

    IDComponent(Entity entity, std::string tag) noexcept;

    Entity GetEntity() const;

    std::string_view GetTag() const;
    std::string& GetTagRef();
    void SetTag(std::string tag);
    void SetTag(std::string&& tag);

    friend void SerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& id);
    friend IDComponent DeserializeIDComponent(tinyxml2::XMLElement* property);
};

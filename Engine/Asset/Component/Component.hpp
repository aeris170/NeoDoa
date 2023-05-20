#pragma once

#include <string>
#include <vector>

struct Component {

    struct Field {
        std::string typeName;
        std::string name;
    };

    std::string name;
    std::string declaration;
    std::vector<Field> fields;

    std::string Serialize() const;
    static Component Deserialize(const std::string_view data);

    static Component Copy(const Component& component);
};
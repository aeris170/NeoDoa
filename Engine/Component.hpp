#pragma once

#include <string>
#include <vector>

struct Component {

    struct Field {
        std::string TypeName;
        std::string Name;
    };

    std::string Name;
    std::string Declaration;
    std::vector<Field> Fields;

    std::string Serialize() const;
    static Component Deserialize(const std::string_view data);

    static Component Copy(const Component& component);
};
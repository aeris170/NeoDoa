#pragma once

#include <string>
#include <string_view>

struct Model {
    std::string Serialize() const noexcept;
    static Model Deserialize(const std::string_view data) noexcept;

    static Model Copy(const Model& model) noexcept;
};

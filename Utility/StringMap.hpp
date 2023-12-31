#pragma once

#include <map>
#include <string>
#include <unordered_map>

namespace detail {
    struct string_hash {
        using is_transparent = void;
        using hash_type = std::hash<std::string_view>;
        [[nodiscard]] size_t operator()(std::string_view txt) const noexcept;
        [[nodiscard]] size_t operator()(const std::string& txt) const noexcept;
        [[nodiscard]] size_t operator()(const char* txt) const noexcept;
    };

    struct string_equal {
        using is_transparent = std::true_type;
        [[nodiscard]] bool operator()(std::string_view l, std::string_view r) const noexcept;
        [[nodiscard]] bool operator()(std::string_view l, const std::string& r) const noexcept;
        [[nodiscard]] bool operator()(std::string_view l, const char* r) const noexcept;

        [[nodiscard]] bool operator()(const std::string& l, std::string_view r) const noexcept;
        [[nodiscard]] bool operator()(const std::string& l, const std::string& r) const noexcept;
        [[nodiscard]] bool operator()(const std::string& l, const char* r) const noexcept;

        [[nodiscard]] bool operator()(const char* l, std::string_view r) const noexcept;
        [[nodiscard]] bool operator()(const char* l, const std::string& r) const noexcept;
        [[nodiscard]] bool operator()(const char* l, const char* r) const noexcept;
    };
}

template<typename V>
using unordered_string_map = std::unordered_map<std::string, V, detail::string_hash, detail::string_equal>;

template<typename V>
using string_map           = std::map          <std::string, V, detail::string_hash, detail::string_equal>;
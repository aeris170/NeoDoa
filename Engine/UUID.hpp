#pragma once

#include <string>
#include <random>

struct UUID {
    constexpr static UUID Empty() { return UUID{ static_cast<uint64_t>(0) }; };

    UUID() noexcept : uuid(uid(mt)) {}
    constexpr UUID(uint64_t uuid) noexcept : uuid(uuid) {}
    explicit UUID(const std::string& string) noexcept;

    std::string AsString() const;

    constexpr operator uint64_t() const { return uuid; }

private:
    static std::random_device rd;
    static std::mt19937_64 mt;
    static std::uniform_int_distribution<uint64_t> uid;

    uint64_t uuid;

    friend struct std::hash<UUID>;
    friend constexpr std::strong_ordering operator<=>(const UUID& id1, const UUID& id2) = default;
};

namespace std {
    template<>
    struct hash<UUID> {
        size_t operator()(const UUID& uuid) const {
            return hash<decltype(uuid.uuid)>()(uuid.uuid);
        }
    };
}

#ifdef _DEBUG
#include <type_traits>
static_assert(std::is_copy_constructible_v<UUID>);
static_assert(std::is_copy_assignable_v<UUID>);
static_assert(std::is_move_constructible_v<UUID>);
static_assert(std::is_move_assignable_v<UUID>);
#endif
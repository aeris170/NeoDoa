#pragma once

#include <string>
#include <random>

struct UUID {
	constexpr static UUID Empty() { return UUID{ static_cast<uint64_t>(0) }; };

	UUID() noexcept : uuid(uid(mt)) {}
	constexpr UUID(uint64_t uuid) noexcept : uuid(uuid) {}
	UUID(const std::string& string) noexcept;
	constexpr UUID(const UUID& other) = default;
	constexpr UUID& operator=(const UUID& other) = default;
	constexpr UUID(UUID&& other) = default;
	constexpr UUID& operator=(UUID&& other) = default;

	std::string AsString() const;

	constexpr operator uint64_t() const { return uuid; }

private:
	static std::random_device rd;
	static std::mt19937_64 mt;
	static std::uniform_int_distribution<uint64_t> uid;

	uint64_t uuid;
	std::string stringValue;

	friend struct std::hash<UUID>;
	friend constexpr bool operator== (const UUID& id1, const UUID& id2);
	friend constexpr bool operator!= (const UUID& id1, const UUID& id2);
	friend constexpr bool operator< (const UUID& id1, const UUID& id2);
	friend constexpr bool operator> (const UUID& id1, const UUID& id2);
	friend constexpr bool operator<= (const UUID& id1, const UUID& id2);
	friend constexpr bool operator>= (const UUID& id1, const UUID& id2);
};

constexpr bool operator== (const UUID& id1, const UUID& id2) { return id1.uuid == id2.uuid; }
constexpr bool operator!= (const UUID& id1, const UUID& id2) { return id1.uuid != id2.uuid; }
constexpr bool operator< (const UUID& id1, const UUID& id2) { return id1.uuid < id2.uuid; }
constexpr bool operator> (const UUID& id1, const UUID& id2) { return id1.uuid > id2.uuid; }
constexpr bool operator<= (const UUID& id1, const UUID& id2) { return id1.uuid <= id2.uuid; }
constexpr bool operator>= (const UUID& id1, const UUID& id2) { return id1.uuid >= id2.uuid; }

namespace std {
	template<>
	struct hash<UUID> {
		size_t operator()(const UUID& uuid) const {
			return hash<decltype(uuid.uuid)>()(uuid.uuid);
		}
	};
}

#include <type_traits>
static_assert(std::is_copy_constructible_v<UUID>);
static_assert(std::is_copy_assignable_v<UUID>);
static_assert(std::is_move_constructible_v<UUID>);
static_assert(std::is_move_assignable_v<UUID>);
#include "UUID.hpp"

UUID::UUID(const std::string& hashee) noexcept :
	uuid(std::hash<std::string>{}(hashee)) {}

std::string UUID::AsString() const { return std::to_string(uuid); }

std::random_device UUID::rd{};
std::mt19937_64 UUID::mt{ rd() };
std::uniform_int_distribution<uint64_t> UUID::uid;
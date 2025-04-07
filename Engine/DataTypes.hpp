#include <span>
#include <memory>
#include <vector>

#include <entt/entt.hpp>

using OwningPointerToRawData = std::unique_ptr<std::byte>;
using NonOwningPointerToRawData = std::byte*;
using NonOwningPointerToConstRawData = const std::byte*;
using RawData = std::vector<std::byte>;
using RawDataView = std::span<const std::byte>;
using RawDataWriteableView = std::span<std::byte>;

using HashedString = entt::hashed_string;
#pragma once

#include <string>

#include <UUID.hpp>
#include <FileNode.hpp>

#include "SVGPathway.hpp"

struct MetaAssetInfo {
    const FNode* file;

    int icon_index;
    const char* fa_icon; /* from IconsFontAwesome6Pro.h */
    std::string svg_icon_key; /* for SVGPathway::Get */

    void* GetSVGIcon(TextureStyle style = TextureStyle::PADDED) const;
};

struct MetaAssetInfoBank {
#ifdef _DEBUG
    using DataStructure = std::unordered_map<const FNode*, MetaAssetInfo>;
#elif
    using DataStructure = entt::dense_hash_map<const FNode*, MetaAssetInfo>;
#endif

    void Clear();
    void Emplace(MetaAssetInfo&& emplacee);
    MetaAssetInfo& GetMetaInfoOf(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent = {});

    DataStructure::iterator begin();
    DataStructure::iterator end();
    DataStructure::const_iterator begin() const;
    DataStructure::const_iterator end() const;

    void SaveToDisk(const std::filesystem::path& path) const;
    void LoadFromDisk(const Project& owner);

private:
    void TryEmplace(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent);

    MetaAssetInfo bankInfo;
    DataStructure metaInfo;
};

#if _DEBUG
#include <type_traits>
static_assert(std::is_default_constructible_v<MetaAssetInfo>);
static_assert(std::is_copy_constructible_v<MetaAssetInfo>);
static_assert(std::is_copy_assignable_v<MetaAssetInfo>);
static_assert(std::is_move_constructible_v<MetaAssetInfo>);
static_assert(std::is_move_assignable_v<MetaAssetInfo>);
static_assert(std::is_aggregate_v<MetaAssetInfo>);

static_assert(std::is_default_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_copy_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_copy_assignable_v<MetaAssetInfoBank>);
static_assert(std::is_move_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_move_assignable_v<MetaAssetInfoBank>);
static_assert(!std::is_aggregate_v<MetaAssetInfoBank>);
#endif
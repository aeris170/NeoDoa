#pragma once

#include <string>

#include <Engine/UUID.hpp>
#include <Engine/Assets.hpp>
#include <Engine/FileNode.hpp>

#include <Editor/SVGPathway.hpp>

struct MetaAssetInfo {
    const FNode* file{ nullptr };

    int icon_index{ -1 };
    const char* fa_icon{ nullptr }; /* from IconsFontAwesome6Pro.h */
    std::string svg_icon_key{}; /* for SVGPathway::Get */

    void* GetSVGIcon(TextureStyle style = TextureStyle::PADDED) const;
};

struct MetaAssetInfoBank {
#ifdef _DEBUG
    using DataStructure = std::unordered_map<const FNode*, MetaAssetInfo>;
#else
    using DataStructure = entt::dense_map<const FNode*, MetaAssetInfo>;
#endif

    static void SaveToDisk(const MetaAssetInfoBank& bank, const FNode& editorMetaFolder) noexcept;
    static void LoadFromDisk(MetaAssetInfoBank& bank, FNode& editorMetaFolder) noexcept;

    MetaAssetInfoBank() noexcept = default;
    MetaAssetInfoBank(const MetaAssetInfoBank& other) noexcept = default;
    MetaAssetInfoBank(MetaAssetInfoBank&& other) noexcept = default;
    MetaAssetInfoBank& operator=(const MetaAssetInfoBank& other) noexcept = default;
    MetaAssetInfoBank& operator=(MetaAssetInfoBank&& other) noexcept = default;

    void Clear();
    void Emplace(MetaAssetInfo&& emplacee);
    MetaAssetInfo& GetMetaInfoOf(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent = {});

    DataStructure::iterator begin() noexcept;
    DataStructure::iterator end() noexcept;
    DataStructure::const_iterator begin() const noexcept;
    DataStructure::const_iterator end() const noexcept;

private:

    static constexpr auto BankFileName{ "metaAssetInfoBank" };

    void TryEmplace(const FNode& file, const MetaAssetInfo& emplaceThisIfAbsent);

    DataStructure metaInfo;
};

#if _DEBUG
#include <type_traits>
static_assert(std::is_default_constructible_v<MetaAssetInfo>);
static_assert(std::is_copy_constructible_v<MetaAssetInfo>);
static_assert(std::is_copy_assignable_v<MetaAssetInfo>);
static_assert(std::is_nothrow_move_constructible_v<MetaAssetInfo>);
static_assert(std::is_nothrow_move_assignable_v<MetaAssetInfo>);
static_assert(std::is_aggregate_v<MetaAssetInfo>);

static_assert(std::is_default_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_copy_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_copy_assignable_v<MetaAssetInfoBank>);
static_assert(std::is_nothrow_move_constructible_v<MetaAssetInfoBank>);
static_assert(std::is_nothrow_move_assignable_v<MetaAssetInfoBank>);
static_assert(!std::is_aggregate_v<MetaAssetInfoBank>);
#endif
#pragma once

#include <concepts>
#include <functional>

#include <Engine/UUID.hpp>

struct Assets;

template<typename Callable>
concept AssetFilter = requires (Callable& c, UUID assetID, const Assets& assets) {
    { std::invoke(c, assetID, assets) } -> std::convertible_to<bool>;
};

template<AssetFilter T1, AssetFilter T2>
auto operator|(const T1& f1, const T2& f2) {
    return [f1, f2](UUID assetID, const Assets& assets) {
        return std::invoke(f1, assetID, assets) || std::invoke(f2, assetID, assets);
    };
}

namespace AssetFilters {
    struct IncludeSceneAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeComponentAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeSamplerAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeTextureAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeShaderAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeShaderProgramAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeMaterialAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeFrameBufferAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct IncludeModelAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };

    struct ExcludeSceneAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeComponentAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeSamplerAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeTextureAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeShaderAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeShaderProgramAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeMaterialAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeFrameBufferAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
    struct ExcludeModelAssets {
        bool operator()(UUID assetID, const Assets& assets) const noexcept;
    };
}

static_assert(AssetFilter<AssetFilters::IncludeSceneAssets        >);
static_assert(AssetFilter<AssetFilters::IncludeComponentAssets    >);
static_assert(AssetFilter<AssetFilters::IncludeSamplerAssets      >);
static_assert(AssetFilter<AssetFilters::IncludeTextureAssets      >);
static_assert(AssetFilter<AssetFilters::IncludeShaderAssets       >);
static_assert(AssetFilter<AssetFilters::IncludeShaderProgramAssets>);
static_assert(AssetFilter<AssetFilters::IncludeMaterialAssets     >);
static_assert(AssetFilter<AssetFilters::IncludeFrameBufferAssets  >);
static_assert(AssetFilter<AssetFilters::IncludeModelAssets        >);

static_assert(AssetFilter<AssetFilters::ExcludeSceneAssets        >);
static_assert(AssetFilter<AssetFilters::ExcludeComponentAssets    >);
static_assert(AssetFilter<AssetFilters::ExcludeSamplerAssets      >);
static_assert(AssetFilter<AssetFilters::ExcludeTextureAssets      >);
static_assert(AssetFilter<AssetFilters::ExcludeShaderAssets       >);
static_assert(AssetFilter<AssetFilters::ExcludeShaderProgramAssets>);
static_assert(AssetFilter<AssetFilters::ExcludeMaterialAssets     >);
static_assert(AssetFilter<AssetFilters::ExcludeFrameBufferAssets  >);
static_assert(AssetFilter<AssetFilters::ExcludeModelAssets        >);
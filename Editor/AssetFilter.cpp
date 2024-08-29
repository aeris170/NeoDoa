#include <Editor/AssetFilter.hpp>

#include <Engine/Assets.hpp>

bool AssetFilters::IncludeSceneAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsScene();
}
bool AssetFilters::IncludeComponentAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsComponentDefinition();
}
bool AssetFilters::IncludeSamplerAssets::operator()(UUID assetID, const  Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsSampler();
}
bool AssetFilters::IncludeTextureAssets::operator()(UUID assetID, const  Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsTexture();
}
bool AssetFilters::IncludeShaderAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsShader();
}
bool AssetFilters::IncludeShaderProgramAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsShaderProgram();
}
bool AssetFilters::IncludeMaterialAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsMaterial();
}
bool AssetFilters::IncludeModelAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    AssetHandle handle = assets.FindAsset(assetID);
    assert(handle.HasValue());
    return handle->IsModel();
}

bool AssetFilters::ExcludeSceneAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    return !IncludeSceneAssets()(assetID, assets);
}
bool AssetFilters::ExcludeComponentAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    return !IncludeComponentAssets()(assetID, assets);
}
bool AssetFilters::ExcludeSamplerAssets::operator()(UUID assetID, const  Assets& assets) const noexcept {
    return !IncludeSamplerAssets()(assetID, assets);
}
bool AssetFilters::ExcludeTextureAssets::operator()(UUID assetID, const  Assets& assets) const noexcept {
    return !IncludeTextureAssets()(assetID, assets);
}
bool AssetFilters::ExcludeShaderAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    return !IncludeShaderAssets()(assetID, assets);
}
bool AssetFilters::ExcludeShaderProgramAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    return !IncludeShaderProgramAssets()(assetID, assets);
}
bool AssetFilters::ExcludeMaterialAssets::operator()(UUID assetID, const  Assets& assets) const noexcept {
    return !IncludeMaterialAssets()(assetID, assets);
}
bool AssetFilters::ExcludeModelAssets::operator()(UUID assetID, const Assets& assets) const noexcept {
    return !IncludeModelAssets()(assetID, assets);
}
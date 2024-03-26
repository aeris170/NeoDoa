#pragma once

#include <variant>

#include <Engine/Scene.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Entity.hpp>
#include <Engine/FileNode.hpp>

#include <Editor/SceneDisplay.hpp>
#include <Editor/ComponentDefinitionDisplay.hpp>
#include <Editor/ShaderDisplay.hpp>
#include <Editor/ShaderProgramDisplay.hpp>
#include <Editor/MaterialDisplay.hpp>
#include <Editor/SamplerDisplay.hpp>

struct Scene;

struct Observer;
struct MetaAssetInfo;

#define DISPLAYABLE Entity, FNode*
using DisplayTarget = std::variant<std::monostate, DISPLAYABLE>;
#undef DISPLAYABLE

struct DisplayTargetRenderer {

    DisplayTargetRenderer(Observer& observer) noexcept;

    void SetDisplayTarget(Entity entity);
    void SetDisplayTarget(FNode& file);
    void ResetDisplayTarget();

    void Render();
private:

    std::reference_wrapper<Observer> observer;
    DisplayTarget displayTarget;
    std::string renderTargetTitleText;

    SceneDisplay sceneDisplay;
    ComponentDefinitionDisplay componentDefinitionDisplay;
    ShaderDisplay shaderDisplay;
    ShaderProgramDisplay shaderProgramDisplay;
    MaterialDisplay materialDisplay;
    SamplerDisplay samplerDisplay;

    void HandleTargetWhenEmpty();
    void HandleTargetWhenEntity(Scene& scene, const Entity entt);
    void HandleTargetWhenFile(FNode& file);
    void RenderIconChangePopup(const FNode& file, MetaAssetInfo& meta);

    void RenderFolderView(FNode& folder);
    void RenderAssetView(AssetHandle h);
    void RenderSceneView(AssetHandle h);
    void RenderComponentDefinitionView(AssetHandle h);
    void RenderShaderView(AssetHandle h);
    void RenderShaderProgramView(AssetHandle h);
    void RenderMaterialView(AssetHandle h);
    void RenderSamplerView(AssetHandle h);
    void RenderTextureView(AssetHandle h);
    void RenderTextView(AssetHandle h);

    void OnProjectUnloaded();
    void OnReimport(Assets& assets);
    void OnAssetDeleted(AssetHandle asset);
    void OnAssetFocused(AssetHandle asset);
    void OnFolderFocused(FNode& folder);
    void OnFocusLost();
    void OnSceneClosed();
    void OnEntityDeleted(Entity entity);
    void OnEntitySelected(Entity entity);
    void OnEntityDeselected();
};
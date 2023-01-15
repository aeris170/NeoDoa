#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>

#include "Entity.hpp"

#include "TemplateUtilities.hpp"

#include "ImGuiExtensions/TextEditor.h"

struct GUI;
struct Scene;
struct FNode;
struct Component;
struct AssetHandle;

#define DISPLAYABLE Entity, FNode*
template<typename T>
concept DisplayableInObserver = concepts::IsAnyOf<T, DISPLAYABLE>;
using DisplayTarget = std::variant<std::monostate, DISPLAYABLE>;
#undef DISPLAYABLE

struct Observer {

    std::reference_wrapper<GUI> gui;

    Observer(GUI& gui) noexcept;

    void Begin(Scene* scene);
    void Render(Scene& scene);
    void End();

    void RenderComponents(Scene& scene, const Entity entt);
    void RenderFolderView(FNode* folder);
    void RenderAssetView(AssetHandle asset);
    void RenderSceneView(AssetHandle sceneAsset);
    void RenderComponentDefinitionView(AssetHandle componentDefAsset);
    void RenderTextView(AssetHandle textAsset);
    void RenderTextureView(AssetHandle textureAsset);

    template<DisplayableInObserver T>
    void SetDisplayTarget(T target) { displayTarget = target; }
    void ResetDisplayTarget();

private:
    DisplayTarget displayTarget;

    struct ComponentDefinitionDisplay {

        static constexpr ImVec4 ERROR_COLOR{ 1.0f, 0.6f, 0.6f, 1.0f };
        static constexpr ImVec4 INFO_COLOR{ 0.7f, 1.0f, 0.7f, 1.0f };
        static constexpr ImVec4 WARNING_COLOR{ 1.0f, 0.5f, 0.1f, 1.0f };

        static void RenderMessagesTable(const AssetHandle componentDefAsset);
        static void RenderFields(const Component& componentDef);
        static void RenderSourceCode(const Component& componentDef);

    private:
        static inline TextEditor TextEditor{};
    };

};

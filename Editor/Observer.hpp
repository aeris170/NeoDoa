#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>

#include "Entity.hpp"

#include "TemplateUtilities.hpp"

#include "MetaAssetInfo.hpp"
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

    explicit Observer(GUI& gui) noexcept;

    bool Begin(Scene* scene);
    void Render(Scene& scene);
    void End();

    template<DisplayableInObserver T>
    void SetDisplayTarget(T target) { displayTarget = target; }
    void ResetDisplayTarget();

private:

    DisplayTarget displayTarget;

    struct DisplayTargetRenderer {
        static void Render(Observer& observer, Scene& scene);
    private:
        static void HandleTargetWhenEmpty();
        static void HandleTargetWhenEntity(const Observer& observer, Scene& scene, const Entity entt);
        static void HandleTargetWhenFile(const Observer& observer, FNode& file);
        static void RenderIconChangePopup(const FNode& file, MetaAssetInfo& meta);

        static void RenderFolderView(const Observer& observer, FNode& folder);
        static void RenderAssetView(const Observer& observer, AssetHandle h);
        static void RenderSceneView(const Observer& observer, AssetHandle h);
        static void RenderComponentDefinitionView(const Observer& observer, AssetHandle h);
        static void RenderTextureView(const Observer& observer, AssetHandle h);
        static void RenderTextView(const Observer& observer, AssetHandle h);
    };

    struct SceneDisplay {
        static void RenderEntities(const Observer& observer, Scene& scene);
        static void RenderSystems(const Observer& observer, const Scene& scene);
    };

    struct ComponentDefinitionDisplay {

        static void RenderMessagesTable(const AssetHandle componentDefAsset);
        static void RenderFields(const Component& componentDef);
        static void RenderSourceCode(const Component& componentDef);

    private:
        static inline TextEditor TextEditor{};
    };

};

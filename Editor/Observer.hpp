#pragma once

#include <string_view>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>

#include <Utility/TemplateUtilities.hpp>

#include <Engine/Assets.hpp>
#include <Engine/Entity.hpp>

#include <Editor/MetaAssetInfo.hpp>
#include <Editor/TextEditor.hpp>

struct GUI;
struct Scene;
struct FNode;
struct Shader;
struct Component;
struct AssetHandle;

#define DISPLAYABLE Entity, FNode*
using DisplayTarget = std::variant<std::monostate, DISPLAYABLE>;
#undef DISPLAYABLE

struct Observer {

    std::reference_wrapper<GUI> gui;

    explicit Observer(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

    void SetDisplayTarget(Entity entity);
    void SetDisplayTarget(FNode& file);
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
        static void RenderShaderView(const Observer& observer, AssetHandle h);
        static void RenderShaderProgramView(const Observer& observer, AssetHandle h);
        static void RenderTextureView(const Observer& observer, AssetHandle h);
        static void RenderTextView(const Observer& observer, AssetHandle h);
    };

    struct SceneDisplay {
        static void RenderEntities(const Observer& observer, Scene& scene);
        static void RenderSystems(const Observer& observer, const Scene& scene);
    };

    struct ComponentDefinitionDisplay {

        static void Init();

        static void SetDisplayTarget(const AssetHandle componentDefAsset);

        static void RenderMessagesTable();
        static void RenderFields();
        static void RenderSourceCode();

    private:
        static inline AssetHandle ComponentDefAsset{};
        static inline TextEditor TextEditor{};
    };

    struct ShaderDisplay {

        static void Init();

        static void SetDisplayTarget(const AssetHandle shaderAsset);

        static void RenderMessagesTable();
        static void RenderFields();
        static void RenderSourceCode();

    private:
        static inline AssetHandle ShaderAsset{};
        static inline TextEditor TextEditor{};
    };

    struct ShaderProgramDisplay {

        static void Init();

        static void SetDisplayTarget(Assets& assets, const AssetHandle shaderProgramAsset);

        static void RenderMessagesTable();
        static void RenderShaders();

    private:
        static constexpr ImVec4 NOT_SET_MANDATORY_COLOR{ 1, 0, 0, 1 };
        static constexpr ImVec4 NOT_SET_OPTIONAL_COLOR{ 1, 1, 0, 1 };
        static constexpr ImVec4 SET_COLOR{ 0, 1, 0, 1 };
        static constexpr std::string_view NOT_SET_MANDATORY_TEXT{ "NOT SET - MANDATORY!" };
        static constexpr std::string_view NOT_SET_OPTIONAL_TEXT{ "NOT SET - OPTIONAL!" };

        static inline Assets* assets{ nullptr };
        static inline AssetHandle ShaderProgramAsset{};
        static inline std::string VertexShaderText;
        static inline std::string TessCtrlShaderText;
        static inline std::string TessEvalShaderText;
        static inline std::string GeometryShaderText;
        static inline std::string FragmentShaderText;

        static void Begin(std::string_view label);
        static void RenderVertexShader(ShaderProgram& program);
        static void RenderTessCtrlShader(ShaderProgram& program);
        static void RenderTessEvalShader(ShaderProgram& program);
        static void RenderGeometryShader(ShaderProgram& program);
        static void RenderFragmentShader(ShaderProgram& program);
        static void End();
    };

    std::string renderTargetTitleText;

    void OnReimport(Assets& assets);
    void OnAssetDeleted(AssetHandle asset);
    void OnAssetFocused(AssetHandle asset);
    void OnFolderFocused(FNode& folder);
    void OnFocusLost();
    void OnSceneClosed();
    void OnEntitySelected(Entity entity);
    void OnEntityDeselected();

};

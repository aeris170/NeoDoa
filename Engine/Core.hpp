#pragma once

#include <tuple>
#include <memory>
#include <functional>
#include <filesystem>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "Angel.hpp"
#include "Input.hpp"
#include "Assets.hpp"
#include "Window.hpp"
#include "Project.hpp"
#include "AssetBridge.hpp"

#include <Engine/Graphics.hpp>

struct Core;
struct Resolution;

using CoreDeleter = std::function<void(Core*)>;
using CorePtr = std::unique_ptr<Core, CoreDeleter>;

struct Core {
    static const CorePtr& CreateCore(GraphicsBackend gBackend, WindowBackend wBackend, const ContextWindowCreationParams& params);
    static const CorePtr& GetCore();
    static void DestroyCore();

    bool IsAnyContextInitialized() const noexcept;
    bool IsSoftwareRendererInitialized() const noexcept;
#if defined(OPENGL_4_6_SUPPORT) || defined(OPENGL_3_3_SUPPORT)
    bool IsOpenGLInitialized() const noexcept;
#endif
#ifdef VULKAN_SUPPORT
    bool IsVulkanInitialized() const noexcept;
#endif
#ifdef DIRECT3D_12_SUPPORT
    bool IsDirect3D12Initialized() const noexcept;
#endif
#ifdef DIRECT3D_11_SUPPORT
    bool IsDirect3D11Initialized() const noexcept;
#endif

    bool IsRunning() const;
    void SetPlaying(bool playing);
    bool IsPlaying() const;

    std::unique_ptr<Angel>& GetAngel();
    std::unique_ptr<IWindow>& GetWindow();
    std::unique_ptr<Input>& GetInput();

    void CreateAndLoadProject(std::string_view workspace, std::string_view name);
    void LoadProject(const std::string& path);
    void LoadProject(const std::filesystem::path& path);
    std::unique_ptr<Project>& LoadedProject();
    void UnloadProject();
    void SaveLoadedProjectToDisk() const;
    bool HasLoadedProject();

    std::unique_ptr<Assets>& GetAssets();
    std::unique_ptr<AssetGPUBridge>& GetAssetGPUBridge();

    void Start();
    void Stop();

private:
    bool running{ false };
    bool playing{ false };

    std::unique_ptr<Angel> angel{};
    std::unique_ptr<IWindow> window{};
    std::unique_ptr<Input> input{};
    std::unique_ptr<Project> project{};
    std::unique_ptr<Assets> assets{};
    std::unique_ptr<AssetGPUBridge> gpuBridge{};

    Core() = default;
    ~Core() = default;
    Core(const Core&) = delete;
    Core(Core&&) = delete;
    Core& operator=(const Core&) = delete;
    Core& operator=(Core&&) = delete;

    static void DeleteCore(Core* core);
    static void HandleNew();

    /* Core Attachment */
public:
    struct Attachment : entt::type_list<void(Project*), void(Project*)> {
        template<typename Base>
        struct type : Base {
            void BeforeFrame(Project* project) { entt::poly_call<0>(*this, project); }
            void AfterFrame(Project* project) { entt::poly_call<1>(*this, project); }
        };

        template<typename Type>
        using impl = entt::value_list<&Type::BeforeFrame, &Type::AfterFrame>;
    };

    template<typename T, typename ...Args>
    T& CreateAttachment(Args&&... args) {
        return *static_cast<T*>(_attachments.emplace(entt::type_id<T>().hash(), T(std::forward<Args>(args)...)).first->second.data());
    }

    template<typename T>
    T& GetAttachment() {
        return *static_cast<T*>(_attachments[entt::type_id<T>().hash()].data());
    }

private:
    entt::dense_map<entt::id_type, entt::poly<Attachment>> _attachments{};
    /* Core Attachment */
};

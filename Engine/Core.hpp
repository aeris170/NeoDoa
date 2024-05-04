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
#include "FrameBuffer.hpp"
#include "AssetBridge.hpp"

struct Core;
struct Resolution;

using CoreDeleter = std::function<void(Core*)>;
using CorePtr = std::unique_ptr<Core, CoreDeleter>;

struct Core {
    static const CorePtr& CreateCore(Resolution resolution, const char* title, bool isFullscreen = false, const char* windowIcon = nullptr, bool renderOffscreen = false);
    static const CorePtr& GetCore();
    static void DestroyCore();

    bool IsRunning() const;
    void SetPlaying(bool playing);
    bool IsPlaying() const;

    std::unique_ptr<Angel>& GetAngel();
    WindowPtr& GetWindow();
    std::unique_ptr<Input>& GetInput();
    std::unique_ptr<FrameBuffer>& GetFrameBuffer();

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
    inline static CorePtr _this{ nullptr };

    bool running{ false };
    bool playing{ false };

    std::unique_ptr<Angel> angel{ nullptr };
    WindowPtr window{ nullptr };
    std::unique_ptr<Input> input{ nullptr };
    std::unique_ptr<FrameBuffer> offscreenBuffer{ nullptr };
    GPUFrameBuffer offscreenBuffer_{};
    std::unique_ptr<Project> project{ nullptr };
    std::unique_ptr<Assets> assets{ nullptr };
    std::unique_ptr<AssetGPUBridge> gpuBridge{ nullptr };

    Core() = default;
    ~Core() = default;
    Core(const Core&) = delete;
    Core(Core&&) = delete;
    Core& operator=(const Core&) = delete;
    Core& operator=(Core&&) = delete;

    static void DeleteCore(Core* core);

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

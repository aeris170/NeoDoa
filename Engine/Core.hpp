#pragma once

#include <tuple>
#include <memory>
#include <functional>

#include <entt.hpp>
#include <glm/glm.hpp>

#include "Window.hpp"

struct Core;
struct Angel;
struct Input;
struct FrameBuffer;
struct Project;
struct Resolution;

using CoreDeleter = std::function<void(Core*)>;
using CorePtr = std::unique_ptr<Core, CoreDeleter>;

struct Core {
    static const CorePtr& CreateCore(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon = nullptr, bool renderOffscreen = false);
    static const CorePtr& GetCore();
    static void DestroyCore();

    bool IsRunning() const;
    void SetPlaying(bool playing);
    bool IsPlaying() const;

    std::unique_ptr<Angel>& Angel();
    WindowPtr& Window();
    std::unique_ptr<Input>& Input();
    std::unique_ptr<FrameBuffer>& FrameBuffer();

    void LoadProject(const Project& project);
    Project* GetLoadedProject() const;
    void UnloadProject();

    void Start();
    void Stop();

private:
    inline static CorePtr _this{ nullptr };

    bool _running{ false };
    bool _playing{ false };

    std::unique_ptr<struct Angel> _angel{ nullptr };
    WindowPtr _window{ nullptr };
    std::unique_ptr<struct Input> _input{ nullptr };
    std::unique_ptr<struct FrameBuffer> _offscreenBuffer{ nullptr };
    Project* _project{ nullptr };

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
    entt::dense_hash_map<entt::id_type, entt::poly<Attachment>> _attachments{};
    //std::vector<entt::poly<Attachment>> _attachments{};
    /* Core Attachment */
};

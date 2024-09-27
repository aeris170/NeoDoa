#pragma once

#include <any>
#include <span>
#include <cstddef>
#include <utility>
#include <string_view>

#include <Engine/Event.hpp>
#include <Engine/Input.hpp>
#include <Engine/Point.hpp>
#include <Engine/Monitor.hpp>
#include <Engine/DataTypes.hpp>
#include <Engine/Resolution.hpp>

enum class WindowBackend {
    GLFW,
#ifdef SDL_SUPPORT
    SDL
#endif
};

enum class CursorType {
    Arrow,
    IBeam,
    XHair,
    PointingHand,
    ResizeEW,
    ResizeNS,
    ResizeNWSE,
    ResizeNESW,
    ResizeAll,
    NotAllowed,
};

struct CursorData {
    Resolution Size{};
    RawData Pixels{};
    Point HotSpot{};
};

struct WindowIconPack {
    struct IconData {
        Resolution Size{};
        RawData Pixels{};
    };
    std::vector<IconData> Icons{};
};

struct ContextWindowCreationParams {
    Resolution Size{ 1, 1 }; // defaults to the smallest possible window size
    std::string_view Title{ "NeoDoa Window" };

    bool IsFullScreen{ false };
    unsigned Monitor{}; // 0 is first monitor

    bool IsResizable{ false };
    bool IsVisible{ false };
    bool IsDecorated{ true };
    bool IsFocused{ true };
    bool IsAutoIconified{ true };
    bool IsAlwaysOnTop{ false };
    bool IsMaximized{ false };
    bool IsCursorCentered{ true };
    bool HasTransparentFrameBuffer{ false };
    bool HasFocusOnShow{ true };
    bool ShouldScaleToMonitor{ false };
    bool IsStereoscopic{ false };
    bool IssRGBCapable{ false };
    bool IsDoubleBuffered{ true };

    int RedBits{ 8 };
    int GreenBits{ 8 };
    int BlueBits{ 8 };
    int AlphaBits{ 8 };
    int DepthBits{ 24 };
    int StencilBits{ 8 };

    int Samples{ 0 };
    int RefreshRate{ -1 }; // -1 is don't care

    WindowIconPack IconPack{};
    CursorData ArrowCursor{};
    CursorData IBeamCursor{};
    CursorData XHairCursor{};
    CursorData PointingHandCursor{};
    CursorData ResizeEWCursor{};
    CursorData ResizeNSCursor{};
    CursorData ResizeNWSECursor{};
    CursorData ResizeNESWCursor{};
    CursorData ResizeAllCursor{};
    CursorData NotAllowedCursor{};
};

struct IWindow {

    enum class Iconify {
        Iconified,
        Restored
    };

    enum class Focus {
        FocusGained,
        FocusLost
    };

    struct Events {
        Event<void()> OnClose{};
        Event<void(Resolution)> OnResize{};
        Event<void(Resolution)> OnFrameBufferResize{};
        Event<void(Point)> OnMove{};
        Event<void(Iconify)> OnIconify{};
        Event<void(Focus)> OnFocus{};
        Event<void()> OnRefresh{};

        Event<void(Key)> OnKeyPress{};
        Event<void(Key)> OnKeyRelease{};
        Event<void(Key)> OnCharInput{};

        Event<void(PointDouble)> OnMouseMove{};
        Event<void()> OnMouseEnter{};
        Event<void()> OnMouseLeave{};
        Event<void(MouseButton)> OnMousePress{};
        Event<void(MouseButton)> OnMouseRelease{};
        Event<void(MouseScroll)> OnMouseScroll{};
        Event<void(std::span<std::string_view>)> OnPathDrop{};
    } Events;

    virtual ~IWindow() noexcept = 0;

    virtual void SetResolution(Resolution resolution) noexcept = 0;
    virtual Resolution GetResolution() const noexcept = 0;
    virtual void SetContentResolution(Resolution resolution) noexcept = 0;
    virtual Resolution GetContentResolution() const noexcept = 0;

    virtual std::string_view GetTitle() const noexcept = 0;
    virtual void SetTitle(std::string_view title) noexcept = 0;

    virtual bool IsFullscreen() const noexcept = 0;
    virtual void SetFullscreen(const IMonitor& monitor, const VideoMode& videoMode) noexcept = 0;
    virtual void SetWindowed(const VideoMode& videoMode) noexcept = 0;

    virtual bool IsResizable() const noexcept = 0;
    virtual void SetResizable(bool isResizable) noexcept = 0;

    virtual bool IsVisible() const noexcept = 0;
    virtual void SetVisible(bool isVisible) noexcept = 0;

    virtual bool IsDecorated() const noexcept = 0;
    virtual void SetDecorated(bool isDecorated) noexcept = 0;

    virtual bool HasFocus() const noexcept = 0;
    virtual void Focus() noexcept = 0;

    virtual bool IsAutoIconified() const noexcept = 0;
    virtual void SetAutoIconified(bool isAutoIconified) noexcept = 0;

    virtual bool IsAlwaysOnTop() const noexcept = 0;
    virtual void SetAlwaysOnTop(bool isAlwaysOnTop) noexcept = 0;

    virtual void Minimize() const noexcept = 0;
    virtual void Restore() const noexcept = 0;
    virtual void Maximize() const noexcept = 0;

    virtual Point GetPosition() const noexcept = 0;
    virtual void SetPosition(Point position) const noexcept = 0;

    virtual void ChangeCursor(CursorType type) noexcept = 0;
    virtual void CaptureCursor() const noexcept = 0;
    virtual void DisableCursor() const noexcept = 0;
    virtual void HideCursor() const noexcept = 0;
    virtual void EnableCursor() const noexcept = 0;

    virtual void SetIcon(const WindowIconPack& iconPack) noexcept = 0;

    virtual void Close() noexcept = 0;
    virtual void PollEvents() const noexcept = 0;
    virtual void SwapBuffers() const noexcept = 0;
    virtual bool ShouldClose() const noexcept = 0;

    virtual WindowBackend GetPlatformBackend() const noexcept = 0;
    virtual std::any GetPlatformWindowPointer() const noexcept = 0;
    virtual void SetPlatformWindowPointerContext(std::any context) noexcept = 0;
    virtual bool IsSoftwareRendererContextWindow() const noexcept = 0;
    virtual bool IsOpenGLContextWindow() const noexcept = 0;
    virtual bool IsVulkanContextWindow() const noexcept = 0;
    virtual bool IsDirect3D12ContextWindow() const noexcept = 0;
    virtual bool IsDirect3D11ContextWindow() const noexcept = 0;
};

constexpr std::string_view ToString(WindowBackend backend) noexcept {
    using enum WindowBackend;
    switch (backend) {
    case GLFW:  return "GLFW";
#ifdef SDL_SUPPORT
    case SDL:   return "SDL";
#endif
    }
    std::unreachable();
}

std::ostream& operator<<(std::ostream& os, WindowBackend backend);
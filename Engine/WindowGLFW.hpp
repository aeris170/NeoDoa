#pragma once

#include <Engine/Window.hpp>
#include <Engine/Graphics.hpp>

struct GLFWwindow;
struct GLFWcursor;

struct WindowGLFW final : public IWindow {

    using PlatformWindowType = GLFWwindow*;

    static void SetIconOfPlatformWindow(std::any platformWindow, const WindowIconPack& iconPack) noexcept;

    WindowGLFW(GraphicsBackend backend, const ContextWindowCreationParams& params) noexcept;
    ~WindowGLFW() noexcept override;
    WindowGLFW(const WindowGLFW&) = delete;
    WindowGLFW(WindowGLFW&& other) noexcept;
    WindowGLFW& operator=(const WindowGLFW&) = delete;
    WindowGLFW& operator=(WindowGLFW&& other) noexcept;

    void SetResolution(Resolution resolution) noexcept override;
    Resolution GetResolution() const noexcept override;
    void SetContentResolution(Resolution resolution) noexcept override;
    Resolution GetContentResolution() const noexcept override;

    std::string_view GetTitle() const noexcept override;
    void SetTitle(std::string_view title) noexcept override;

    bool IsFullscreen() const noexcept override;
    void SetFullscreen(const IMonitor& monitor, const VideoMode& videoMode) noexcept override;
    void SetWindowed(const VideoMode& videoMode) noexcept override;

    bool IsResizable() const noexcept override;
    void SetResizable(bool isResizable) noexcept override;

    bool IsVisible() const noexcept override;
    void SetVisible(bool isVisible) noexcept override;

    bool IsDecorated() const noexcept override;
    void SetDecorated(bool isDecorated) noexcept override;

    bool HasFocus() const noexcept override;
    void Focus() noexcept override;

    bool IsAutoIconified() const noexcept override;
    void SetAutoIconified(bool isAutoIconified) noexcept override;

    bool IsAlwaysOnTop() const noexcept override;
    void SetAlwaysOnTop(bool isAlwaysOnTop) noexcept override;

    void Minimize() const noexcept override;
    void Restore() const noexcept override;
    void Maximize() const noexcept override;

    Point GetPosition() const noexcept override;
    void SetPosition(Point position) const noexcept override;

    void ChangeCursor(CursorType type) noexcept override;
    void CaptureCursor() const noexcept override;
    void DisableCursor() const noexcept override;
    void HideCursor() const noexcept override;
    void EnableCursor() const noexcept override;

    void SetIcon(const WindowIconPack& iconPack) noexcept override;

    void Close() noexcept override;
    void PollEvents() const noexcept override;
    void SwapBuffers() const noexcept override;
    bool ShouldClose() const noexcept override;

    WindowBackend GetPlatformBackend() const noexcept override;
    std::any GetPlatformWindowPointer() const noexcept override;
    void SetPlatformWindowPointerContext(std::any context) noexcept override;
    bool IsSoftwareRendererContextWindow() const noexcept override;
    bool IsOpenGLContextWindow() const noexcept override;
    bool IsVulkanContextWindow() const noexcept override;
    bool IsDirect3D12ContextWindow() const noexcept override;
    bool IsDirect3D11ContextWindow() const noexcept override;

private:

    static GLFWcursor* CreateCursor(const CursorData& dataToCreateFrom, int glfwConstantToFallbackTo) noexcept;
    static Key ConvertGLFWKey(int glfwKey) noexcept;

    GLFWwindow* glfwWindow{};
    GLFWcursor* arrowCursor{};
    GLFWcursor* iBeamCursor{};
    GLFWcursor* xHairCursor{};
    GLFWcursor* pointingHandCursor{};
    GLFWcursor* resizeEWCursor{};
    GLFWcursor* resizeNSCursor{};
    GLFWcursor* resizeNWSECursor{};
    GLFWcursor* resizeNESWCursor{};
    GLFWcursor* resizeAllCursor{};
    GLFWcursor* notAllowedCursor{};

    bool isSoftware{};
    bool isOpenGL{};
    bool isOpenGL46{};
    bool isOpenGL33{};
    bool isVulkan{};
    bool isDirect3D12{};
    bool isDirect3D11{};

    Point windowedPos;
};
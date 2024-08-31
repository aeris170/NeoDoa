#include <Engine/WindowGLFW.hpp>

#include <cassert>
#include <utility>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <Engine/Log.hpp>

void WindowGLFW::SetIconOfPlatformWindow(std::any platformWindow, const WindowIconPack& iconPack) noexcept {
    if (iconPack.Icons.empty()) { return; }

    std::vector<GLFWimage> icons{};
    icons.reserve(iconPack.Icons.size());
    for (const WindowIconPack::IconData& data : iconPack.Icons) {
        icons.emplace_back(
            data.Size.Width,
            data.Size.Height,
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data.Pixels.data()))
        );
    }
    glfwSetWindowIcon(std::any_cast<decltype(WindowGLFW::glfwWindow)>(platformWindow), static_cast<int>(icons.size()), icons.data());
}

WindowGLFW::WindowGLFW(GraphicsBackend backend, const ContextWindowCreationParams& params) noexcept {
    glfwWindowHint(GLFW_RESIZABLE,               params.IsResizable               ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE,                 params.IsVisible                 ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED,               params.IsDecorated               ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING,                params.IsAlwaysOnTop             ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED,               params.IsMaximized               ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CENTER_CURSOR,           params.IsCursorCentered          ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, params.HasTransparentFrameBuffer ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW,           params.HasFocusOnShow            ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR,        params.ShouldScaleToMonitor      ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_STEREO,                  params.IsStereoscopic            ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SRGB_CAPABLE,            params.IssRGBCapable             ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER,            params.IsDoubleBuffered          ? GLFW_TRUE : GLFW_FALSE);

    glfwWindowHint(GLFW_RED_BITS,     params.RedBits);
    glfwWindowHint(GLFW_GREEN_BITS,   params.GreenBits);
    glfwWindowHint(GLFW_BLUE_BITS,    params.BlueBits);
    glfwWindowHint(GLFW_ALPHA_BITS,   params.AlphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS,   params.DepthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, params.StencilBits);

    glfwWindowHint(GLFW_SAMPLES,      params.Samples);
    glfwWindowHint(GLFW_REFRESH_RATE, params.RefreshRate);

    isSoftware = backend == GraphicsBackend::Software;
#ifdef OPENGL_4_6_SUPPORT
    isOpenGL  |= backend == GraphicsBackend::OpenGL4_6;
    isOpenGL46 = backend == GraphicsBackend::OpenGL4_6;
#endif
#ifdef OPENGL_3_3_SUPPORT
    isOpenGL  |= backend == GraphicsBackend::OpenGL3_3;
    isOpenGL33 = backend == GraphicsBackend::OpenGL3_3;
#endif
#ifdef VULKAN_SUPPORT
    isVulkan = backend == GraphicsBackend::Vulkan;
#endif
#ifdef DIRECT3D_12_SUPPORT
    isDirect3D12 = backend == GraphicsBackend::Direct3D12;
#endif
#ifdef DIRECT3D_11_SUPPORT
    isDirect3D11 = backend == GraphicsBackend::Direct3D11;
#endif


    if (isOpenGL) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
        if (isOpenGL46) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        } else if (isOpenGL33) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        } else {
            std::unreachable();
        }
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);
#elif NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#else
#error "Neither DEBUG nor NDEBUG are defined!"
#endif
    }

    GLFWmonitor* monitor{};
    if (params.IsFullScreen) {
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        assert(params.Monitor < count);
        monitor = monitors[params.Monitor];
    }

    glfwWindow = glfwCreateWindow(params.Size.Width, params.Size.Height, params.Title.data(), monitor, nullptr);
    if (!glfwWindow) {
        const char* errorMessage;
        glfwGetError(&errorMessage);
        DOA_LOG_FATAL("Failed to create GLFW window. Reason: %s", errorMessage);
        std::unreachable();
    }

    glfwSetWindowUserPointer(glfwWindow, this);
    if (isOpenGL) {
        glfwMakeContextCurrent(glfwWindow);
    }

    SetIcon(params.IconPack);
    arrowCursor        = CreateCursor(params.ArrowCursor,        GLFW_ARROW_CURSOR);
    iBeamCursor        = CreateCursor(params.IBeamCursor,        GLFW_IBEAM_CURSOR);
    xHairCursor        = CreateCursor(params.XHairCursor,        GLFW_CROSSHAIR_CURSOR);
    pointingHandCursor = CreateCursor(params.PointingHandCursor, GLFW_POINTING_HAND_CURSOR);
    resizeEWCursor     = CreateCursor(params.ResizeEWCursor,     GLFW_RESIZE_EW_CURSOR);
    resizeNSCursor     = CreateCursor(params.ResizeNSCursor,     GLFW_RESIZE_NS_CURSOR);
    resizeNWSECursor   = CreateCursor(params.ResizeNWSECursor,   GLFW_RESIZE_NWSE_CURSOR);
    resizeNESWCursor   = CreateCursor(params.ResizeNESWCursor,   GLFW_RESIZE_NESW_CURSOR);
    resizeAllCursor    = CreateCursor(params.ResizeAllCursor,    GLFW_RESIZE_ALL_CURSOR);
    notAllowedCursor   = CreateCursor(params.NotAllowedCursor,   GLFW_NOT_ALLOWED_CURSOR);

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnClose();
    });
    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnResize({
            static_cast<decltype(Resolution::Width)>(width),
            static_cast<decltype(Resolution::Height)>(height)
        });
    });
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnFrameBufferResize({
            static_cast<decltype(Resolution::Width)>(width),
            static_cast<decltype(Resolution::Height)>(height)
        });
    });
    glfwSetWindowPosCallback(glfwWindow, [](GLFWwindow* window, int xpos, int ypos) {
        WindowGLFW* dis = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        dis->windowedPos.X = static_cast<decltype(decltype(windowedPos)::X)>(xpos);
        dis->windowedPos.Y = static_cast<decltype(decltype(windowedPos)::Y)>(ypos);
        dis->Events.OnMove(dis->windowedPos);
    });
    glfwSetWindowIconifyCallback(glfwWindow, [](GLFWwindow* window, int iconified) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnIconify({
            iconified == GLFW_TRUE ? Iconify::Iconified : Iconify::Restored
        });
    });
    glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* window, int focused) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnFocus({
            focused == GLFW_TRUE ? Focus::FocusGained : Focus::FocusLost
        });
    });
    glfwSetWindowRefreshCallback(glfwWindow, [](GLFWwindow* window) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnRefresh();
    });

    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
        if (action == GLFW_PRESS) {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnKeyPress(ConvertGLFWKey(key));
        }
        if (action == GLFW_RELEASE) {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnKeyRelease(ConvertGLFWKey(key));
        }
    });

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMouseMove({
            static_cast<decltype(PointDouble::X)>(xpos),
            static_cast<decltype(PointDouble::Y)>(ypos)
        });
    });
    glfwSetCursorEnterCallback(glfwWindow, [](GLFWwindow* window, int entered) {
        if (entered == GLFW_TRUE) {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMouseEnter();
        } else {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMouseLeave();
        }
    });
    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
        using enum MouseButton;
        MouseButton mouseButton{};
        if (button == GLFW_MOUSE_BUTTON_1) { mouseButton = Left; }
        if (button == GLFW_MOUSE_BUTTON_2) { mouseButton = Right; }
        if (button == GLFW_MOUSE_BUTTON_3) { mouseButton = Middle; }
        if (button == GLFW_MOUSE_BUTTON_4) { mouseButton = Mouse4; }
        if (button == GLFW_MOUSE_BUTTON_5) { mouseButton = Mouse5; }
        if (button == GLFW_MOUSE_BUTTON_6) { mouseButton = Mouse6; }
        if (button == GLFW_MOUSE_BUTTON_7) { mouseButton = Mouse7; }
        if (button == GLFW_MOUSE_BUTTON_8) { mouseButton = Mouse8; }

        if (action == GLFW_PRESS) {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMousePress(mouseButton);
        } else {
            static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMouseRelease(mouseButton);
        }
    });
    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnMouseScroll({
            static_cast<decltype(MouseScroll::XOffset)>(xoffset),
            static_cast<decltype(MouseScroll::YOffset)>(yoffset)
        });
    });
    glfwSetDropCallback(glfwWindow, [](GLFWwindow* window, int path_count, const char* paths[]) {
        std::string_view* views = static_cast<std::string_view*>(alloca(path_count * sizeof(std::string_view)));
        std::transform(paths, paths + path_count, views, [](const char* path) -> std::string_view {
            return { path };
        });

        static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window))->Events.OnPathDrop({ views, static_cast<std::size_t>(path_count) });
    });
}
WindowGLFW::~WindowGLFW() noexcept {
    glfwDestroyWindow(glfwWindow);
}
WindowGLFW::WindowGLFW(WindowGLFW&& other) noexcept {
    *this = std::move(other);
}
WindowGLFW& WindowGLFW::operator=(WindowGLFW&& other) noexcept {
    std::swap(glfwWindow, other.glfwWindow);
    return *this;
}

void WindowGLFW::SetResolution(Resolution resolution) noexcept {
    glfwSetWindowSize(glfwWindow, resolution.Width, resolution.Height);
}
Resolution WindowGLFW::GetResolution() const noexcept {
    int w, h;
    glfwGetWindowSize(glfwWindow, &w, &h);
    return {
        static_cast<decltype(Resolution::Width)>(w),
        static_cast<decltype(Resolution::Height)>(h)
    };
}
void WindowGLFW::SetContentResolution(Resolution resolution) noexcept {
    int left, top, right, bottom;
    glfwGetWindowFrameSize(glfwWindow, &left, &top, &right, &bottom);
    glfwSetWindowSize(glfwWindow, resolution.Width + left + right, resolution.Height + top + bottom);
}
Resolution WindowGLFW::GetContentResolution() const noexcept {
    int w, h;
    glfwGetFramebufferSize(glfwWindow, &w, &h);
    return {
        static_cast<decltype(Resolution::Width)>(w),
        static_cast<decltype(Resolution::Height)>(h)
    };
}

std::string_view WindowGLFW::GetTitle() const noexcept {
    return glfwGetWindowTitle(glfwWindow);
}
void WindowGLFW::SetTitle(std::string_view title) noexcept {
    glfwSetWindowTitle(glfwWindow, title.data());
}

bool WindowGLFW::IsFullscreen() const noexcept {
    return glfwGetWindowMonitor(glfwWindow) != nullptr;
}
void WindowGLFW::SetFullscreen(const IMonitor& monitor, const VideoMode& videoMode) noexcept {
    int xpos, ypos;
    glfwGetWindowPos(glfwWindow, &xpos, &ypos);
    windowedPos.X = static_cast<decltype(decltype(windowedPos)::X)>(xpos);
    windowedPos.Y = static_cast<decltype(decltype(windowedPos)::Y)>(ypos);

    [[maybe_unused]] int count;
    glfwSetWindowMonitor(glfwWindow, glfwGetMonitors(&count)[monitor.GetIndex()], 0, 0, videoMode.Size.Width, videoMode.Size.Height, videoMode.RefreshRate);
}
void WindowGLFW::SetWindowed(const VideoMode& videoMode) noexcept {
    glfwSetWindowMonitor(glfwWindow, nullptr, windowedPos.X, windowedPos.Y, videoMode.Size.Width, videoMode.Size.Height, videoMode.RefreshRate);
}

bool WindowGLFW::IsResizable() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_RESIZABLE) == GLFW_TRUE;
}
void WindowGLFW::SetResizable(bool isResizable) noexcept {
    glfwSetWindowAttrib(glfwWindow, GLFW_RESIZABLE, isResizable ? GLFW_TRUE : GLFW_FALSE);
}

bool WindowGLFW::IsVisible() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_VISIBLE) == GLFW_TRUE;
}
void WindowGLFW::SetVisible(bool isVisible) noexcept {
    glfwSetWindowAttrib(glfwWindow, GLFW_VISIBLE, isVisible ? GLFW_TRUE : GLFW_FALSE);
}

bool WindowGLFW::IsDecorated() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_DECORATED) == GLFW_TRUE;
}
void WindowGLFW::SetDecorated(bool isDecorated) noexcept {
    glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, isDecorated ? GLFW_TRUE : GLFW_FALSE);
}

bool WindowGLFW::HasFocus() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_FOCUSED) == GLFW_TRUE;
}
void WindowGLFW::Focus() noexcept {
    glfwFocusWindow(glfwWindow);
}

bool WindowGLFW::IsAutoIconified() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_AUTO_ICONIFY) == GLFW_TRUE;
}
void WindowGLFW::SetAutoIconified(bool isAutoIconified) noexcept {
    return glfwSetWindowAttrib(glfwWindow, GLFW_AUTO_ICONIFY, isAutoIconified ? GLFW_TRUE : GLFW_FALSE);
}

bool WindowGLFW::IsAlwaysOnTop() const noexcept {
    return glfwGetWindowAttrib(glfwWindow, GLFW_FLOATING) == GLFW_TRUE;
}
void WindowGLFW::SetAlwaysOnTop(bool isAlwaysOnTop) noexcept {
    glfwSetWindowAttrib(glfwWindow, GLFW_FLOATING, isAlwaysOnTop ? GLFW_TRUE : GLFW_FALSE);
}

void WindowGLFW::Minimize() const noexcept { glfwIconifyWindow(glfwWindow); }
void WindowGLFW::Restore() const noexcept { glfwRestoreWindow(glfwWindow); }
void WindowGLFW::Maximize() const noexcept { glfwMaximizeWindow(glfwWindow); }

Point WindowGLFW::GetPosition() const noexcept {
    int x, y;
    glfwGetWindowPos(glfwWindow, &x, &y);
    return {
        static_cast<decltype(Point::X)>(x),
        static_cast<decltype(Point::Y)>(y)
    };
}
void WindowGLFW::SetPosition(Point position) const noexcept {
    glfwSetWindowPos(glfwWindow, position.X, position.Y);
}

void WindowGLFW::ChangeCursor(CursorType type) noexcept {
    using enum CursorType;
    switch (type) {
    case Arrow:        glfwSetCursor(glfwWindow, arrowCursor);        return;
    case IBeam:        glfwSetCursor(glfwWindow, iBeamCursor);        return;
    case XHair:        glfwSetCursor(glfwWindow, xHairCursor);        return;
    case PointingHand: glfwSetCursor(glfwWindow, pointingHandCursor); return;
    case ResizeEW:     glfwSetCursor(glfwWindow, resizeEWCursor);     return;
    case ResizeNS:     glfwSetCursor(glfwWindow, resizeNSCursor);     return;
    case ResizeNWSE:   glfwSetCursor(glfwWindow, resizeNWSECursor);   return;
    case ResizeNESW:   glfwSetCursor(glfwWindow, resizeNESWCursor);   return;
    case ResizeAll:    glfwSetCursor(glfwWindow, resizeAllCursor);    return;
    case NotAllowed:   glfwSetCursor(glfwWindow, notAllowedCursor);   return;
    }
    std::unreachable();
}
void WindowGLFW::CaptureCursor() const noexcept {
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}
void WindowGLFW::DisableCursor() const noexcept {
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void WindowGLFW::HideCursor() const noexcept {
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}
void WindowGLFW::EnableCursor() const noexcept {
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowGLFW::SetIcon(const WindowIconPack& iconPack) noexcept {
    SetIconOfPlatformWindow(glfwWindow, iconPack);
}

void WindowGLFW::Close() noexcept {
    glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
}
void WindowGLFW::PollEvents() const noexcept {
    glfwPollEvents();
}
void WindowGLFW::SwapBuffers() const noexcept {
    glfwSwapBuffers(glfwWindow);
}
bool WindowGLFW::ShouldClose() const noexcept {
    return glfwWindowShouldClose(glfwWindow);
}

WindowBackend WindowGLFW::GetPlatformBackend() const noexcept {
    return WindowBackend::GLFW;
}
std::any WindowGLFW::GetPlatformWindowPointer() const noexcept {
    return glfwWindow;
}
void WindowGLFW::SetPlatformWindowPointerContext(std::any context) noexcept {
    auto* ctx = std::any_cast<decltype(glfwWindow)>(context);
    glfwWindow = ctx;
    glfwMakeContextCurrent(ctx);
}
bool WindowGLFW::IsSoftwareRendererContextWindow() const noexcept {
    return isSoftware;
}
bool WindowGLFW::IsOpenGLContextWindow() const noexcept {
    return isOpenGL;
}
bool WindowGLFW::IsVulkanContextWindow() const noexcept {
    return isVulkan;
}
bool WindowGLFW::IsDirect3D12ContextWindow() const noexcept {
    return isDirect3D12;
}
bool WindowGLFW::IsDirect3D11ContextWindow() const noexcept {
    return isDirect3D11;
}

GLFWcursor* WindowGLFW::CreateCursor(const CursorData& data, int glfwConstantToFallbackTo) noexcept {
    if (data.Size == Resolution{}) { return glfwCreateStandardCursor(glfwConstantToFallbackTo); }

    GLFWimage cursor;
    cursor.width = data.Size.Width;
    cursor.height = data.Size.Height;
    cursor.pixels = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data.Pixels.data()));
    return glfwCreateCursor(&cursor, data.HotSpot.X, data.HotSpot.Y);
}

Key WindowGLFW::ConvertGLFWKey(int glfwKey) noexcept {
    using enum Key;
    switch (glfwKey) {
    case GLFW_KEY_UNKNOWN: return Unknown;
    case GLFW_KEY_SPACE:   return Space;

    case GLFW_KEY_A: return A;
    case GLFW_KEY_B: return B;
    case GLFW_KEY_C: return C;
    case GLFW_KEY_D: return D;
    case GLFW_KEY_E: return E;
    case GLFW_KEY_F: return F;
    case GLFW_KEY_G: return G;
    case GLFW_KEY_H: return H;
    case GLFW_KEY_I: return I;
    case GLFW_KEY_J: return J;
    case GLFW_KEY_K: return K;
    case GLFW_KEY_L: return L;
    case GLFW_KEY_M: return M;
    case GLFW_KEY_N: return N;
    case GLFW_KEY_O: return O;
    case GLFW_KEY_P: return P;
    case GLFW_KEY_Q: return Q;
    case GLFW_KEY_R: return R;
    case GLFW_KEY_S: return S;
    case GLFW_KEY_T: return T;
    case GLFW_KEY_U: return U;
    case GLFW_KEY_V: return V;
    case GLFW_KEY_W: return W;
    case GLFW_KEY_X: return X;
    case GLFW_KEY_Y: return Y;
    case GLFW_KEY_Z: return Z;

    case GLFW_KEY_0: return Num0;
    case GLFW_KEY_1: return Num1;
    case GLFW_KEY_2: return Num2;
    case GLFW_KEY_3: return Num3;
    case GLFW_KEY_4: return Num4;
    case GLFW_KEY_5: return Num5;
    case GLFW_KEY_6: return Num6;
    case GLFW_KEY_7: return Num7;
    case GLFW_KEY_8: return Num8;
    case GLFW_KEY_9: return Num9;

    case GLFW_KEY_F1:  return F1;
    case GLFW_KEY_F2:  return F2;
    case GLFW_KEY_F3:  return F3;
    case GLFW_KEY_F4:  return F4;
    case GLFW_KEY_F5:  return F5;
    case GLFW_KEY_F6:  return F6;
    case GLFW_KEY_F7:  return F7;
    case GLFW_KEY_F8:  return F8;
    case GLFW_KEY_F9:  return F9;
    case GLFW_KEY_F10: return F10;
    case GLFW_KEY_F11: return F11;
    case GLFW_KEY_F12: return F12;
    case GLFW_KEY_F13: return F13;
    case GLFW_KEY_F14: return F14;
    case GLFW_KEY_F15: return F15;
    case GLFW_KEY_F16: return F16;
    case GLFW_KEY_F17: return F17;
    case GLFW_KEY_F18: return F18;
    case GLFW_KEY_F19: return F19;
    case GLFW_KEY_F20: return F20;
    case GLFW_KEY_F21: return F21;
    case GLFW_KEY_F22: return F22;
    case GLFW_KEY_F23: return F23;
    case GLFW_KEY_F24: return F24;
    case GLFW_KEY_F25: return F25;

    case GLFW_KEY_APOSTROPHE:    return Apostrophe;
    case GLFW_KEY_COMMA:         return Comma;
    case GLFW_KEY_MINUS:         return Minus;
    case GLFW_KEY_PERIOD:        return Period;
    case GLFW_KEY_SLASH:         return Slash;
    case GLFW_KEY_SEMICOLON:     return Semicolon;
    case GLFW_KEY_EQUAL:         return Equal;
    case GLFW_KEY_LEFT_BRACKET:  return LeftBracket;
    case GLFW_KEY_BACKSLASH:     return Backslash;
    case GLFW_KEY_RIGHT_BRACKET: return RightBracket;
    case GLFW_KEY_GRAVE_ACCENT:  return GraveAccent;

    case GLFW_KEY_UP:    return ArrowUp;
    case GLFW_KEY_DOWN:  return ArrowDown;
    case GLFW_KEY_LEFT:  return ArrowLeft;
    case GLFW_KEY_RIGHT: return ArrowRight;

    case GLFW_KEY_ESCAPE:    return Escape;
    case GLFW_KEY_ENTER:     return Enter;
    case GLFW_KEY_TAB:       return Tab;
    case GLFW_KEY_BACKSPACE: return Backspace;
    case GLFW_KEY_INSERT:    return Insert;
    case GLFW_KEY_DELETE:    return Delete;
    case GLFW_KEY_HOME:      return Home;
    case GLFW_KEY_END:       return End;
    case GLFW_KEY_PAGE_UP:   return PageUp;
    case GLFW_KEY_PAGE_DOWN: return PageDown;

    case GLFW_KEY_LEFT_SHIFT:    return LeftShift;
    case GLFW_KEY_RIGHT_SHIFT:   return RightShift;
    case GLFW_KEY_LEFT_CONTROL:  return LeftControl;
    case GLFW_KEY_RIGHT_CONTROL: return RightControl;
    case GLFW_KEY_LEFT_ALT:      return LeftAlt;
    case GLFW_KEY_RIGHT_ALT:     return RightAlt;
    case GLFW_KEY_LEFT_SUPER:    return LeftSuper;
    case GLFW_KEY_RIGHT_SUPER:   return RightSuper;

    case GLFW_KEY_CAPS_LOCK:    return CapsLock;
    case GLFW_KEY_SCROLL_LOCK:  return ScrollLock;
    case GLFW_KEY_NUM_LOCK:     return NumLock;
    case GLFW_KEY_PRINT_SCREEN: return PrintScreen;
    case GLFW_KEY_PAUSE:        return Pause;

    case GLFW_KEY_KP_0:        return Keypad0;
    case GLFW_KEY_KP_1:        return Keypad1;
    case GLFW_KEY_KP_2:        return Keypad2;
    case GLFW_KEY_KP_3:        return Keypad3;
    case GLFW_KEY_KP_4:        return Keypad4;
    case GLFW_KEY_KP_5:        return Keypad5;
    case GLFW_KEY_KP_6:        return Keypad6;
    case GLFW_KEY_KP_7:        return Keypad7;
    case GLFW_KEY_KP_8:        return Keypad8;
    case GLFW_KEY_KP_9:        return Keypad9;
    case GLFW_KEY_KP_DECIMAL:  return KeypadDecimal;
    case GLFW_KEY_KP_DIVIDE:   return KeypadDivide;
    case GLFW_KEY_KP_MULTIPLY: return KeypadMultiply;
    case GLFW_KEY_KP_SUBTRACT: return KeypadSubtract;
    case GLFW_KEY_KP_ADD:      return KeypadAdd;
    case GLFW_KEY_KP_ENTER:    return KeypadEnter;
    case GLFW_KEY_KP_EQUAL:    return KeypadEqual;

    case GLFW_KEY_MENU:    return Menu;
    case GLFW_KEY_WORLD_1: return World1;
    case GLFW_KEY_WORLD_2: return World2;

    default: return Unknown;
    }
}
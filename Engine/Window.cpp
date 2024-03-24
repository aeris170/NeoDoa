#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Core.hpp"
#include "Input.hpp"
#include "ImGuiRenderer.hpp"

const Resolution& Window::GetResolution() const { return _resolution; }
const Resolution& Window::GetContentResolution() const { return _contentResolution; }

std::string Window::GetTitle() const { return _title; }
void Window::SetTitle(const std::string& title) { SetTitle(std::string(title)); }
void Window::SetTitle(std::string&& title) {
    _title = std::move(title);
    glfwSetWindowTitle(_glfwWindow, _title.c_str());
}

void Window::SetDecorated(bool isDecorated) const {
    glfwHideWindow(_glfwWindow);
    glfwSetWindowAttrib(_glfwWindow, GLFW_DECORATED, isDecorated ? GLFW_TRUE : GLFW_FALSE);
    glfwShowWindow(_glfwWindow);
}

PlatformWindow* Window::GetPlatformWindow() const { return _glfwWindow; }
PlatformUI* Window::GetPlatformUI() const { return _imGuiContext; }

void Window::DisableCursor() const { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void Window::HideCursor() const { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }
void Window::EnableCursor() const { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

void Window::Minimize() const { glfwIconifyWindow(_glfwWindow); }
void Window::Maximize() const { glfwMaximizeWindow(_glfwWindow); };

glm::vec2 Window::GetPosition() const {
    int xpos, ypos;
    glfwGetWindowPos(_glfwWindow, &xpos, &ypos);
    return { xpos, ypos };
}
void Window::SetPosition(glm::vec2 pos) const {
    glfwSetWindowPos(_glfwWindow, pos.x, pos.y);
}
//-----------------------------------------------------------------

WindowPtr Window::CreateWindow(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon) {
    auto rv = std::unique_ptr<Window, std::function<void(Window*)>>(new Window, DeleteWindow);
    rv->_resolution = resolution;
    rv->_title = title;
    rv->_isFullscreen = isFullscreen;

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); //TODO add support
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    rv->_glfwWindow = glfwCreateWindow(rv->_resolution.Width, rv->_resolution.Height, title, isFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    std::array<int, 2> contentResolution;
    glfwGetWindowSize(rv->_glfwWindow, &contentResolution[0], &contentResolution[1]);
    rv->_contentResolution.Width = contentResolution[0];
    rv->_contentResolution.Height = contentResolution[1];

    glfwMakeContextCurrent(rv->_glfwWindow);
    //glfwSwapInterval(0); // TODO VSYNC

    glfwSetFramebufferSizeCallback(rv->_glfwWindow, glfwWindowOnResize);
    glfwSetKeyCallback(rv->_glfwWindow, glfwWindowOnKeyStateChange);
    glfwSetMouseButtonCallback(rv->_glfwWindow, glfwWindowOnMouseButtonStateChange);
    glfwSetCursorPosCallback(rv->_glfwWindow, glfwWindowOnMouseMove);
    glfwSetScrollCallback(rv->_glfwWindow, glfwWindowOnMouseScroll);

    rv->_imGuiContext = ImGuiInit(rv->_glfwWindow);
    ImGui::SetCurrentContext(rv->_imGuiContext);

#pragma region Window Icon Initialization
    if (windowIcon) {
        std::vector<GLFWimage> windowIcons{};
        windowIcons.resize(6);

        std::string str;
        str.reserve(256);

        str.append(windowIcon);
        str.append("-16_x_16.png");
        windowIcons[0].pixels = stbi_load(str.c_str(), &windowIcons[0].width, &windowIcons[0].height, 0, 4);

        str.resize(0);
        str.append(windowIcon);
        str.append("-32_x_32.png");
        windowIcons[1].pixels = stbi_load(str.c_str(), &windowIcons[1].width, &windowIcons[1].height, 0, 4);

        str.resize(0);
        str.append(windowIcon);
        str.append("-48_x_48.png");
        windowIcons[2].pixels = stbi_load(str.c_str(), &windowIcons[2].width, &windowIcons[2].height, 0, 4);

        str.resize(0);
        str.append(windowIcon);
        str.append("-64_x_64.png");
        windowIcons[3].pixels = stbi_load(str.c_str(), &windowIcons[3].width, &windowIcons[3].height, 0, 4);

        str.resize(0);
        str.append(windowIcon);
        str.append("-128_x_128.png");
        windowIcons[4].pixels = stbi_load(str.c_str(), &windowIcons[4].width, &windowIcons[4].height, 0, 4);

        str.resize(0);
        str.append(windowIcon);
        str.append("-256_x_256.png");
        windowIcons[5].pixels = stbi_load(str.c_str(), &windowIcons[5].width, &windowIcons[5].height, 0, 4);

        glfwSetWindowIcon(rv->_glfwWindow, windowIcons.size(), windowIcons.data());
        ImGuiSetUpWindowIcons(std::move(windowIcons));
    }
#pragma endregion

    return rv;
}

void Window::DeleteWindow(Window* window) {
    ImGuiClean();
    glfwDestroyWindow(window->_glfwWindow);
    glfwTerminate();

    delete window;
}

void Window::glfwWindowOnResize(GLFWwindow* window, int width, int height) {
    static const auto& Window = Core::GetCore()->GetWindow();
    Window->_resolution = { static_cast<size_t>(width), static_cast<size_t>(height) };
    std::array<int, 2> contentResolution;
    glfwGetWindowSize(Window->_glfwWindow, &contentResolution[0], &contentResolution[1]);
    Window->_contentResolution.Width = contentResolution[0];
    Window->_contentResolution.Height = contentResolution[1];
}

void Window::glfwWindowOnKeyStateChange(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static const auto& input = Core::GetCore()->GetInput();
    input->keyboard.Keys[key] = action;
}

void Window::glfwWindowOnMouseButtonStateChange(GLFWwindow* window, int button, int action, int mods) {
    static const auto& input = Core::GetCore()->GetInput();
    input->mouse.Buttons[button] = action;
}

void Window::glfwWindowOnMouseMove(GLFWwindow* window, double xpos, double ypos) {
    static const auto& input = Core::GetCore()->GetInput();
    input->mouse.PosX = xpos;
    input->mouse.PosY = ypos;
}

void Window::glfwWindowOnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    static const auto& input = Core::GetCore()->GetInput();
    input->mouse.ScrollX = xoffset;
    input->mouse.ScrollY = yoffset;
}

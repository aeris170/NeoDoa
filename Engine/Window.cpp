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

PlatformWindow* Window::GetPlatformWindow() { return _glfwWindow; }

void Window::DisableCursor() { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void Window::HideCursor() { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }
void Window::EnableCursor() { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
//-----------------------------------------------------------------

WindowPtr Window::CreateWindow(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon) {
    auto rv = std::unique_ptr<Window, std::function<void(Window*)>>(new Window, DeleteWindow);
    rv->_resolution = resolution;
    rv->_title = title;
    rv->_isFullscreen = isFullscreen;

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); //TODO add support
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    rv->_glfwWindow = glfwCreateWindow(rv->_resolution.Width, rv->_resolution.Height, title, isFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    glfwGetWindowSize(rv->_glfwWindow, &rv->_contentResolution.Width, &rv->_contentResolution.Height);

#pragma region Window Icon Initialization
    GLFWimage img[6];
    std::string str;
    str.reserve(256);
    str.append(windowIcon);
    str.append("-16_x_16.png");
    img[0].pixels = stbi_load(str.c_str(), &img[0].width, &img[0].height, 0, 4);
    str.resize(0);
    str.append(windowIcon);
    str.append("-32_x_32.png");
    img[1].pixels = stbi_load(str.c_str(), &img[1].width, &img[1].height, 0, 4);
    str.resize(0);
    str.append(windowIcon);
    str.append("-48_x_48.png");
    img[2].pixels = stbi_load(str.c_str(), &img[2].width, &img[2].height, 0, 4);
    str.resize(0);
    str.append(windowIcon);
    str.append("-64_x_64.png");
    img[3].pixels = stbi_load(str.c_str(), &img[3].width, &img[3].height, 0, 4);
    str.resize(0);
    str.append(windowIcon);
    str.append("-128_x_128.png");
    img[4].pixels = stbi_load(str.c_str(), &img[4].width, &img[4].height, 0, 4);
    str.resize(0);
    str.append(windowIcon);
    str.append("-256_x_256.png");
    img[5].pixels = stbi_load(str.c_str(), &img[5].width, &img[5].height, 0, 4);
    glfwSetWindowIcon(rv->_glfwWindow, 6, img);
    stbi_image_free(img[0].pixels);
    stbi_image_free(img[1].pixels);
    stbi_image_free(img[2].pixels);
    stbi_image_free(img[3].pixels);
    stbi_image_free(img[4].pixels);
    stbi_image_free(img[5].pixels);
#pragma endregion

    glfwMakeContextCurrent(rv->_glfwWindow);
    //glfwSwapInterval(0); // TODO VSYNC

    glfwSetFramebufferSizeCallback(rv->_glfwWindow, glfwWindowOnResize);
    glfwSetKeyCallback(rv->_glfwWindow, glfwWindowOnKeyStateChange);
    glfwSetMouseButtonCallback(rv->_glfwWindow, glfwWindowOnMouseButtonStateChange);
    glfwSetCursorPosCallback(rv->_glfwWindow, glfwWindowOnMouseMove);
    glfwSetScrollCallback(rv->_glfwWindow, glfwWindowOnMouseScroll);

    rv->_imGuiContext = ImGuiInit(rv->_glfwWindow);
    ImGui::SetCurrentContext(rv->_imGuiContext);

    return rv;
}

void Window::DeleteWindow(Window* window) {
    ImGuiClean();
    glfwDestroyWindow(window->_glfwWindow);
    glfwTerminate();

    delete window;
}

void Window::glfwWindowOnResize(GLFWwindow* window, int width, int height) {
    static auto& Window = Core::GetCore()->Window();
    Window->_resolution = { width, height };
    glfwGetWindowSize(Window->_glfwWindow, &Window->_contentResolution.Width, &Window->_contentResolution.Height);
}

void Window::glfwWindowOnKeyStateChange(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static auto& input = Core::GetCore()->Input();
    input->Keyboard.Keys[key] = action;
}

void Window::glfwWindowOnMouseButtonStateChange(GLFWwindow* window, int button, int action, int mods) {
    static auto& input = Core::GetCore()->Input();
    input->Mouse.Buttons[button] = action;
}

void Window::glfwWindowOnMouseMove(GLFWwindow* window, double xpos, double ypos) {
    static auto& input = Core::GetCore()->Input();
    input->Mouse.PosX = xpos;
    input->Mouse.PosY = ypos;
}

void Window::glfwWindowOnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    static auto& input = Core::GetCore()->Input();
    input->Mouse.ScrollX = xoffset;
    input->Mouse.ScrollY = yoffset;
}

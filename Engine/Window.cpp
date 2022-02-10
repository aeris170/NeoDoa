#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "ImGuiRenderer.hpp"

static void glfwWindowOnResize(GLFWwindow* window, int width, int height);
static void glfwWindowOnKeyStateChange(GLFWwindow* window, int key, int scancode, int action, int mods);
static void glfwWindowOnMouseButtonStateChange(GLFWwindow* window, int button, int action, int mods);
static void glfwWindowOnMouseMove(GLFWwindow* window, double xpos, double ypos);
static void glfwWindowOnMouseScroll(GLFWwindow* window, double width, double height);

Window::Window(int width, int height, const char* title, bool isFullscreen, const char* windowIcon) noexcept :
    _width(width),
    _height(height),
    _title(title),
    _isFullscreen(isFullscreen),
    _mouse({}),
    _keyboard({}) {

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); //TODO add support
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    _glfwWindow = glfwCreateWindow(width, height, title, isFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    glfwGetWindowSize(_glfwWindow, &_content_width, &_content_height);

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
    glfwSetWindowIcon(_glfwWindow, 6, img);
    stbi_image_free(img[0].pixels);
    stbi_image_free(img[1].pixels);
    stbi_image_free(img[2].pixels);
    stbi_image_free(img[3].pixels);
    stbi_image_free(img[4].pixels);
    stbi_image_free(img[5].pixels);
    #pragma endregion

    glfwMakeContextCurrent(_glfwWindow);

    glfwSetWindowUserPointer(_glfwWindow, this);
    glfwSetFramebufferSizeCallback(_glfwWindow, glfwWindowOnResize);
    glfwSetKeyCallback(_glfwWindow, glfwWindowOnKeyStateChange);
    glfwSetMouseButtonCallback(_glfwWindow, glfwWindowOnMouseButtonStateChange);
    glfwSetCursorPosCallback(_glfwWindow, glfwWindowOnMouseMove);
    glfwSetScrollCallback(_glfwWindow, glfwWindowOnMouseScroll);

    _imGuiContext = ImGuiInit(_glfwWindow);
}

Window::~Window() noexcept {
    ImGuiClean();
    glfwTerminate();
}

std::string Window::GetTitle() const {
    return _title;
}

void Window::SetTitle(const std::string& title) {
    _title = title;
    glfwSetWindowTitle(_glfwWindow, title.c_str());
}

void Window::SetTitle(std::string&& title) {
    _title = std::move(title);
    glfwSetWindowTitle(_glfwWindow, title.c_str());
}

void Window::DisableCursor() {
    glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::HideCursor() {
    glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window::EnableCursor() {
    glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
//-----------------------------------------------------------------

static void glfwWindowOnResize(GLFWwindow* window, int width, int height) {
    //glViewport(0, 0, width, height);
    Window* w = ((Window*)glfwGetWindowUserPointer(window));
    w->_width = width;
    w->_height = height;
    glfwGetWindowSize(w->_glfwWindow, &w->_content_width, &w->_content_height);
}

static void glfwWindowOnKeyStateChange(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ((Window*)glfwGetWindowUserPointer(window))->_keyboard._keys[key] = action;
}

static void glfwWindowOnMouseButtonStateChange(GLFWwindow* window, int button, int action, int mods) {
    ((Window*)glfwGetWindowUserPointer(window))->_mouse._buttons[button] = action;
}

static void glfwWindowOnMouseMove(GLFWwindow* window, double xpos, double ypos) {
    ((Window*)glfwGetWindowUserPointer(window))->_mouse._posx = xpos;
    ((Window*)glfwGetWindowUserPointer(window))->_mouse._posy = ypos;
}

static void glfwWindowOnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    ((Window*)glfwGetWindowUserPointer(window))->_mouse._scrollx = xoffset;
    ((Window*)glfwGetWindowUserPointer(window))->_mouse._scrolly = yoffset;
}

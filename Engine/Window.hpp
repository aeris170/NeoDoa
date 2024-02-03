#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "TypedefsAndConstants.hpp"
#include "Resolution.hpp"

struct Window;
struct Project;
struct GLFWwindow;
struct ImGuiContext;

using WindowPtr = std::unique_ptr<Window, std::function<void(Window*)>>;
using PlatformWindow = GLFWwindow;
using PlatformUI = ImGuiContext;

struct Window {

    const Resolution& GetResolution() const;
    const Resolution& GetContentResolution() const;

    std::string GetTitle() const;
    void SetTitle(const std::string& title);
    void SetTitle(std::string&& title);

    void SetDecorated(bool isDecorated) const;

    PlatformWindow* GetPlatformWindow() const;
    PlatformUI* GetPlatformUI() const;

    void DisableCursor() const;
    void HideCursor() const;
    void EnableCursor() const;

    void Minimize() const;
    void Maximize() const;

    glm::vec2 GetPosition() const;
    void SetPosition(glm::vec2 pos) const;

private:
    Resolution _resolution{};
    Resolution _contentResolution{};
    std::string _title{};
    bool _isFullscreen{ false };
    GLFWwindow* _glfwWindow{ nullptr };
    ImGuiContext* _imGuiContext{ nullptr };

    Window() = default;
    ~Window() = default;
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    friend struct Core;
    static WindowPtr CreateWindow(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon);
    static void DeleteWindow(Window* window);

    static void glfwWindowOnResize(GLFWwindow* window, int width, int height);
    static void glfwWindowOnKeyStateChange(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwWindowOnMouseButtonStateChange(GLFWwindow* window, int button, int action, int mods);
    static void glfwWindowOnMouseMove(GLFWwindow* window, double xpos, double ypos);
    static void glfwWindowOnMouseScroll(GLFWwindow* window, double width, double height);
};

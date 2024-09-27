#pragma once

#include <Engine/Monitor.hpp>

struct GLFWmonitor;
struct GLFWvidmode;

struct MonitorGLFW : public IMonitor {

    unsigned GetIndex() const noexcept override;

    Point GetPosition() const noexcept override;
    Region GetPositionWorkArea() const noexcept override;

    std::string_view GetName() const noexcept override;

    std::span<const VideoMode> GetVideoModes() const noexcept override;
    VideoMode GetCurrentVideoMode() const noexcept override;

private:

    static VideoMode GLFWvidmodeToVideoMode(const GLFWvidmode& vidMode) noexcept;

    MonitorGLFW(unsigned index, GLFWmonitor* glfwMonitor) noexcept;

    unsigned index{};
    GLFWmonitor* glfwMonitor{};
    std::vector<VideoMode> videoModes{};

    friend struct Monitors;
};
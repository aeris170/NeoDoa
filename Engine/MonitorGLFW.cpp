#include <Engine/MonitorGLFW.hpp>

#include <GLFW/glfw3.h>

MonitorGLFW::MonitorGLFW(unsigned index, GLFWmonitor* glfwMonitor) noexcept :
    index(index),
    glfwMonitor(glfwMonitor) {
    int count;
    const GLFWvidmode* modes = glfwGetVideoModes(glfwMonitor, &count);
    for (int i = 0; i < count; i++) {
        videoModes.emplace_back(GLFWvidmodeToVideoMode(modes[i]));
    }
}

unsigned MonitorGLFW::GetIndex() const noexcept { return index; }

Point MonitorGLFW::GetPosition() const noexcept {
    int xpos, ypos;
    glfwGetMonitorPos(glfwMonitor, &xpos, &ypos);

    return {
        static_cast<decltype(Point::X)>(xpos),
        static_cast<decltype(Point::Y)>(ypos),
    };
}
Region MonitorGLFW::GetPositionWorkArea() const noexcept {
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(glfwMonitor, &xpos, &ypos, &width, &height);

    return {
        static_cast<decltype(Region::X)>(xpos),
        static_cast<decltype(Region::Y)>(ypos),
        static_cast<decltype(Region::Width)>(width),
        static_cast<decltype(Region::Height)>(height),
    };
}

std::string_view MonitorGLFW::GetName() const noexcept { return glfwGetMonitorName(glfwMonitor); }

std::span<const VideoMode> MonitorGLFW::GetVideoModes() const noexcept { return videoModes; }
VideoMode MonitorGLFW::GetCurrentVideoMode() const noexcept { return GLFWvidmodeToVideoMode(*glfwGetVideoMode(glfwMonitor)); }

VideoMode MonitorGLFW::GLFWvidmodeToVideoMode(const GLFWvidmode& vidMode) noexcept {
    return {
        Resolution{
            static_cast<decltype(Resolution::Width)>(vidMode.width),
            static_cast<decltype(Resolution::Height)>(vidMode.height)
        },
        vidMode.redBits,
        vidMode.greenBits,
        vidMode.blueBits,
        vidMode.refreshRate
    };
}
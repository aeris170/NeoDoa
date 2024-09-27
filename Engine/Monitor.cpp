#include <Engine/Monitor.hpp>

#include <cassert>

#include <GLFW/glfw3.h>

#include <Engine/Log.hpp>
#include <Engine/MonitorGLFW.hpp>

IMonitor::~IMonitor() noexcept = default;

Monitors::MonitorList::MonitorList(std::vector<std::unique_ptr<IMonitor>>& monitors) noexcept : monitors(monitors) {}
Monitors::MonitorList::Iterator<IMonitor> Monitors::MonitorList::begin() { return { std::to_address(monitors.begin()) }; }
Monitors::MonitorList::Iterator<IMonitor> Monitors::MonitorList::end()   { return { std::to_address(monitors.end())   }; }

IMonitor& Monitors::PrimaryMonitor() noexcept { assert(!monitors.empty()); return *monitors[0]; }
Monitors::MonitorList Monitors::AllMonitors() noexcept { return monitors; }

void Monitors::Initialize() noexcept {
    assert(!initialized);
    initialized = true;
    if (!glfwInit()) {
        const char* errorMessage;
        glfwGetError(&errorMessage);
        DOA_LOG_FATAL("GLFW couldn't initialize! Reason: %s", errorMessage);
        std::abort();
    }

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    DOA_LOG_INFO("Initialized GLFW %i.%i.%i for Monitor queries.", major, minor, revision);

    int count;
    auto** glfwMonitors = glfwGetMonitors(&count);

    monitors.reserve(count);
    for (int i = 0; i < count; i++) {
        MonitorGLFW* monitor = new(std::nothrow) MonitorGLFW(i, glfwMonitors[i]);
        monitors.emplace_back(monitor);
    }
}

void Monitors::DeInitialize() noexcept {
    assert(initialized);
    monitors.clear();
    glfwTerminate();
}

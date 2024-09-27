#pragma once

#include <span>
#include <memory>
#include <vector>

#include <Engine/Point.hpp>
#include <Engine/Region.hpp>
#include <Engine/Resolution.hpp>

struct VideoMode {
    Resolution Size{ 1, 1 }; // Must set
    int RedBits{ 8 };        // Can ignore safely
    int GreenBits{ 8 };      // Can ignore safely
    int BlueBits{ 8 };       // Can ignore safely
    int RefreshRate{ -1 };   // -1 is "Don't care"
};

struct IMonitor {

    virtual ~IMonitor() noexcept = 0;

    virtual unsigned GetIndex() const noexcept = 0;

    virtual Point GetPosition() const noexcept = 0;
    virtual Region GetPositionWorkArea() const noexcept = 0;

    virtual std::string_view GetName() const noexcept = 0;

    virtual std::span<const VideoMode> GetVideoModes() const noexcept = 0;
    virtual VideoMode GetCurrentVideoMode() const noexcept = 0;
};

struct Monitors {
    struct MonitorList {
        template <typename T>
        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;
            using smart = const std::unique_ptr<std::remove_const_t<T>>;

            Iterator(smart* ptr) : ptr(ptr) {}

            reference operator*() const { return *ptr; }
            pointer operator->() { return ptr; }
            Iterator<T>& operator++() { ptr++; return *this; }
            Iterator<T> operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator==(const Iterator<T>& a, const Iterator<T>& b) = default;

        private:
            smart* ptr;
        };

        MonitorList(std::vector<std::unique_ptr<IMonitor>>& monitors) noexcept;

        Iterator<IMonitor> begin();
        Iterator<IMonitor> end();

    private:
        std::vector<std::unique_ptr<IMonitor>>& monitors;
    };

    static IMonitor& PrimaryMonitor() noexcept;
    static MonitorList AllMonitors() noexcept;

private:
    static inline bool initialized{};
    static inline std::vector<std::unique_ptr<IMonitor>> monitors{};

    static void Initialize() noexcept;
    static void DeInitialize() noexcept;

    friend struct Core;
};
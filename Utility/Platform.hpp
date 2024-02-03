#pragma once

#include <Submodules/detector/detector.hpp>

enum class Platform {
    Windows,
    Linux,
    MacOS,
    iOS,
    Android,
    UNIX
};

Platform consteval GetCurrentPlatform() noexcept {
    using enum Platform;
    if constexpr (detect::is_windows_v) {
        return Windows;
    }
    if constexpr (detect::is_linux_v) {
        return Linux;
    }
    if constexpr (detect::is_macos_v) {
        return MacOS;
    }
    if constexpr (detect::is_ios_v) {
        return iOS;
    }
    if constexpr (detect::is_android_v) {
        return Android;
    }
    if constexpr (detect::is_unix_v) {
        return UNIX;
    }
    static_assert("Unsupported platdform.");
}
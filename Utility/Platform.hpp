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
    } else if constexpr (detect::is_linux_v) {
        return Linux;
    } else if constexpr (detect::is_macos_v) {
        return MacOS;
    } else if constexpr (detect::is_ios_v) {
        return iOS;
    } else if constexpr (detect::is_android_v) {
        return Android;
    } else if constexpr (detect::is_unix_v) {
        return UNIX;
    } else {
		static_assert(detect::is_windows_v
			|| detect::is_linux_v
			|| detect::is_macos_v
			|| detect::is_ios_v
			|| detect::is_android_v
			|| detect::is_unix_v,
			"Unsupported platform."
		);
	}
}
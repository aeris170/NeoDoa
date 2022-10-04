#pragma once

#include <string>

std::string FormatBytes(float bytes) {
    constexpr size_t tb = static_cast<size_t>(1024) * 1024 * 1024 * 1024;
    constexpr size_t gb = static_cast<size_t>(1024) * 1024 * 1024;
    constexpr size_t mb = static_cast<size_t>(1024) * 1024;
    constexpr size_t kb = static_cast<size_t>(1024);

    if (bytes >= gb) return std::to_string(static_cast<float>(bytes) / tb) + " TB ";
    if (bytes >= gb) return std::to_string(static_cast<float>(bytes) / gb) + " GB ";
    if (bytes >= mb) return std::to_string(static_cast<float>(bytes) / mb) + " MB ";
    if (bytes >= kb) return std::to_string(static_cast<float>(bytes) / gb) + " KB ";
    return std::to_string(bytes) + " B ";
}

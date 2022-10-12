#pragma once

#include <string>
#include <format>
#include <iomanip>

namespace detail {
    class comma_numpunct : public std::numpunct<char> {
    protected:
        virtual char do_thousands_sep() const { return ','; }
        virtual std::string do_grouping() const { return "\03"; }
    };

    inline std::string comma_format(float bytes) {
        static std::locale comma_locale(std::locale(), new comma_numpunct());

        std::stringstream ss;
        ss.imbue(comma_locale);
        ss << std::setprecision(0) << std::fixed << bytes;
        return ss.str();
    }
}

inline std::string FormatBytes(float bytes) {
    constexpr size_t tb = static_cast<size_t>(1024) * 1024 * 1024 * 1024;
    constexpr size_t gb = static_cast<size_t>(1024) * 1024 * 1024;
    constexpr size_t mb = static_cast<size_t>(1024) * 1024;
    constexpr size_t kb = static_cast<size_t>(1024);

    if (bytes >= gb) return std::format("{0:.1f} {1} ({2} bytes)", bytes / tb, "TB", detail::comma_format(bytes));
    if (bytes >= gb) return std::format("{0:.1f} {1} ({2} bytes)", bytes / gb, "GB", detail::comma_format(bytes));
    if (bytes >= mb) return std::format("{0:.1f} {1} ({2} bytes)", bytes / mb, "MB", detail::comma_format(bytes));
    if (bytes >= kb) return std::format("{0:.1f} {1} ({2} bytes)", bytes / kb, "KB", detail::comma_format(bytes));
    return std::format("{0} bytes", detail::comma_format(bytes));
}
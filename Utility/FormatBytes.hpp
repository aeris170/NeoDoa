#pragma once

#include <string>
#include <iomanip>

namespace detail {
    class comma_numpunct : public std::numpunct<char> {
    protected:
        virtual char do_thousands_sep() const;
        virtual std::string do_grouping() const;
    };

    std::string comma_format(float bytes);
}

std::string FormatBytes(float bytes);
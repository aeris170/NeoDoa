#pragma once

#include <iostream>

struct Region {
    size_t X{};
    size_t Y{};
    size_t Width{};
    size_t Height{};

    bool operator==(const Region& other) const noexcept = default;

    friend std::ostream& operator<<(std::ostream& os, const Region& r);
};
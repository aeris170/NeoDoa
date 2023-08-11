#pragma once

#include <iostream>

struct Region {
    int X{};
    int Y{};
    int Width{};
    int Height{};

    bool operator==(const Region& other) const noexcept = default;

    friend std::ostream& operator<<(std::ostream& os, const Region& r);
};
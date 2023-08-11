#pragma once

#include <iostream>

struct Resolution {
    int Width{};
    int Height{};

    bool operator==(const Resolution& other) const noexcept = default;

    inline float Aspect() const { return static_cast<float>(Width) / static_cast<float>(Height); }

    friend std::ostream& operator<<(std::ostream& os, const Resolution& r);
};
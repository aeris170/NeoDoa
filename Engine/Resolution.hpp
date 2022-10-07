#pragma once

struct Resolution {
    int w, h;

    inline float Aspect() const { return static_cast<float>(w) / static_cast<float>(h); }
};
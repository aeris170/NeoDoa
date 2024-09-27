#pragma once

struct Region {
    unsigned X{};
    unsigned Y{};
    unsigned Width{};
    unsigned Height{};

    bool operator==(const Region& other) const noexcept = default;
};
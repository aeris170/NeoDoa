#pragma once

#include <iostream>

struct Point {
    unsigned X{};
    unsigned Y{};

    bool operator==(const Point& other) const noexcept = default;

    friend std::ostream& operator<<(std::ostream& os, const Point& r);
};

struct PointDouble {
    double X{};
    double Y{};

    bool operator==(const PointDouble& other) const noexcept = default;

    friend std::ostream& operator<<(std::ostream& os, const Point& r);
};
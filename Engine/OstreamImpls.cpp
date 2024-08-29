#include <glm/gtx/string_cast.hpp>

#include <Engine/AABB.hpp>
#include <Engine/Point.hpp>
#include <Engine/Vertex.hpp>
#include <Engine/Region.hpp>
#include <Engine/Resolution.hpp>

std::ostream& operator<<(std::ostream& os, const AABB& aabb) {
    return os << "AABB [Min=" << glm::to_string(aabb.Min) << ", Max=" << glm::to_string(aabb.Max) << "]";
}

std::ostream& operator <<(std::ostream& os, const Point& p) {
    return os << "Point [X=" << p.X << ", Y=" << p.Y << "]";
}

std::ostream& operator <<(std::ostream& os, const PointDouble& p) {
    return os << "Point [X=" << p.X << ", Y=" << p.Y << "]";
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    return os << "Vertex [\tPosition=" << glm::to_string(v.Position) << ",\n\tNormal= " << glm::to_string(v.Normal) << ",\n\tUV=" << glm::to_string(v.TexCoords) << "\n]";
}

std::ostream& operator <<(std::ostream& os, const Region& r) {
    return os << "Region [X=" << r.X << ", Y=" << r.Y << ", Width=" << r.Width << ", Height=" << r.Height << "]";
}

std::ostream& operator <<(std::ostream& os, const Resolution& r) {
    return os << "Resolution [Width=" << r.Width << ", Height=" << r.Height << "]";
}
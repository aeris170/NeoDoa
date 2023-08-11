#include <glm/gtx/string_cast.hpp>

#include <Engine/AABB.hpp>
#include <Engine/Vertex.hpp>
#include <Engine/Region.hpp>
#include <Engine/Resolution.hpp>

std::ostream& operator <<(std::ostream& os, const Resolution& r) {
    return os << "Resolution [Width=" << r.Width << ", Height=" << r.Height << "]";
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    return os << "Vertex [Position=" << glm::to_string(v.Position) << ", Normal= " << glm::to_string(v.Normal) << ", UV=" << glm::to_string(v.TexCoords);
}

std::ostream& operator <<(std::ostream& os, const Region& r) {
    return os << "Region [X=" << r.X << ", Y=" << r.Y << ", Width=" << r.Width << ", Height=" << r.Height << "]";
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb) {
    return os << "AABB [Min=" << glm::to_string(aabb.Min) << ", Max=" << glm::to_string(aabb.Max);
}
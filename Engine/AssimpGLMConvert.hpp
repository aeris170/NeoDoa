#pragma once

#include <assimp/color4.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

inline glm::vec4 aiColorToGLMVec(const aiColor4D& c) {
	return { c.r, c.g, c.b, c.a };
}
inline glm::vec2 aiVecToGLMVec(const aiVector2D& v) {
	return { v.x, v.y };
}
inline glm::vec3 aiVecToGLMVec(const aiVector3D& v) {
	return { v.x, v.y, v.z };
}
inline glm::mat4 aiMat3ToGLMMat3(const aiMatrix3x3& m) {
	return glm::transpose(glm::make_mat3(&m.a1));
}
inline glm::mat4 aiMat4ToGLMMat4(const aiMatrix4x4& m) {
	return glm::transpose(glm::make_mat4(&m.a1));
}
inline glm::quat aiQuatToGLMQuat(const aiQuaternion& q) {
	return { q.w, q.x, q.y, q.z };
}
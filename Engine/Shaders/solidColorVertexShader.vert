#version 450 core

layout (location = 4) in vec3 pos;
layout (location = 5) in vec3 normal;
layout (location = 6) in vec4 color;
layout (location = 7) in vec2 uv;
layout (location = 8) in int texIndex;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

void main() {
	gl_Position = viewProjMatrix * modelMatrix * vec4(pos.x, pos.y, pos.z, 1.0);
}
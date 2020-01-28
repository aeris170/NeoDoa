#version 330

in vec3 vertex_coords;
in vec2 texture_coords;

out vec2 tex_coords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	tex_coords = texture_coords;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_coords, 1.0);
}
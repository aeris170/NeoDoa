#version 330

in vec3 vertex_coords;
in vec4 vertex_color;
in vec2 texture_coords;

out vec4 color;
out vec2 tex_coords;
out vec3 frag_position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	color = vertex_color;
	tex_coords = texture_coords;
	frag_position = vec3(modelMatrix * vec4(vertex_coords, 1.0));
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_coords, 1.0);
}
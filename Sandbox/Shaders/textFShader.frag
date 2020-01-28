#version 330

in vec2 tex_coords;

out vec4 gl_FragColor;

uniform sampler2D characterTextureAtlas;

uniform vec3 color;
uniform vec3 outlineColor;
uniform vec2 outlineOffset;

uniform float width;
uniform float edge;

uniform float borderWidth;
uniform float borderEdge;

void main() {
	float dist = 1.0 - texture(characterTextureAtlas, tex_coords).a;
	float alpha = 1.0 - smoothstep(width, width + edge + 0.0000001, dist);

	float outlineDistance = 1.0 - texture(characterTextureAtlas, tex_coords + outlineOffset).a;
	float outlineAlpha = 1.0 - smoothstep(borderWidth, borderWidth + borderEdge + 0.0000001, outlineDistance);

	float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
	vec3 overallColor = mix(outlineColor, color, alpha / overallAlpha);

	gl_FragColor = vec4(overallColor, overallAlpha);
}
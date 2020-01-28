#version 330

#define MAX_LIGHTS 50

in vec4 color;
in vec2 tex_coords;
in vec3 frag_position;

out vec4 gl_FragColor;

uniform bool applyColors;
uniform bool applyTexture;
uniform sampler2D textureSample;

uniform vec3 ambientLightColor;

uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform float lightRadii[MAX_LIGHTS];

void main() {
	const float linear_term = .0014f;
	const float quad_term = .000007f;

	if(applyTexture) {
		gl_FragColor = texture(textureSample, tex_coords);
		if(applyColors) {
			gl_FragColor = gl_FragColor * color;
		}
	} else if(applyColors) {
		gl_FragColor = color;
	} else {
		gl_FragColor = vec4(.5f, 0.f, .5f, 0.f);
	}
	vec3 totalColor = vec3(0, 0, 0);
	for(int i = 0; i < numLights; ++i) {
		float dist = length(lightPositions[i] - frag_position);
		if(dist < lightIntensities[i] * lightRadii[i]) {
			totalColor = lightColors[i];
		} else if(dist < lightRadii[i]) { 
			totalColor = lightColors[i] * vec3((lightRadii[i] - dist) / lightRadii[i]);
		}
	}
	gl_FragColor *= vec4(ambientLightColor + totalColor, 1);
}
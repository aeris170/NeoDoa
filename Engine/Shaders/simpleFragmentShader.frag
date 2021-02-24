#version 330 core

layout (location = 0) out vec4 FragColor;

in vec4 v_color;
in vec2 v_uv;
flat in int v_texIndex;

uniform int useColors = 1;
uniform int useTexture = 1;
uniform sampler2D missing;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;
uniform sampler2D texture9;
uniform sampler2D texture10;
uniform sampler2D texture11;
uniform sampler2D texture12;
uniform sampler2D texture13;
uniform sampler2D texture14;
uniform sampler2D texture15;
uniform sampler2D texture16;

void main() {
    vec4 colorContrib = useColors == 1 ? v_color : vec4(1, 1, 1, 1);

    vec4 texContrib = vec4(1, 1, 1, 1);
    if(useTexture == 1) {
		switch(v_texIndex) {
		case 0: texContrib = texture(texture1, v_uv); break;
		case 1: texContrib = texture(texture2, v_uv); break;
		case 2: texContrib = texture(texture3, v_uv); break;
		case 3: texContrib = texture(texture4, v_uv); break;
		case 4: texContrib = texture(texture5, v_uv); break;
		case 5: texContrib = texture(texture6, v_uv); break;
		case 6: texContrib = texture(texture7, v_uv); break;
		case 7: texContrib = texture(texture8, v_uv); break;
		case 8: texContrib = texture(texture9, v_uv); break;
		case 9: texContrib = texture(texture10, v_uv); break;
		case 10: texContrib = texture(texture11, v_uv); break;
		case 11: texContrib = texture(texture12, v_uv); break;
		case 12: texContrib = texture(texture13, v_uv); break;
		case 13: texContrib = texture(texture14, v_uv); break;
		case 14: texContrib = texture(texture15, v_uv); break;
		case 15: texContrib = texture(texture16, v_uv); break;
		default: texContrib = texture(missing, v_uv); break;
		}
    }
    FragColor = colorContrib * texContrib;
}

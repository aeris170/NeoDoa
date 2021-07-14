#version 450 core

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

	vec4 tex1    = texture(texture1,  v_uv);
	vec4 tex2    = texture(texture2,  v_uv);
	vec4 tex3    = texture(texture3,  v_uv);
	vec4 tex4    = texture(texture4,  v_uv);
	vec4 tex5    = texture(texture5,  v_uv);
	vec4 tex6    = texture(texture6,  v_uv);
	vec4 tex7    = texture(texture7,  v_uv);
	vec4 tex8    = texture(texture8,  v_uv);
	vec4 tex9    = texture(texture9,  v_uv);
	vec4 tex10   = texture(texture10, v_uv);
	vec4 tex11   = texture(texture11, v_uv);
	vec4 tex12   = texture(texture12, v_uv);
	vec4 tex13   = texture(texture13, v_uv);
	vec4 tex14   = texture(texture14, v_uv);
	vec4 tex15   = texture(texture15, v_uv);
	vec4 tex16   = texture(texture16, v_uv);
	vec4 missing = texture(missing,   v_uv);

	switch(v_texIndex) {
	case 0:  texContrib = tex1;    break;
	case 1:  texContrib = tex2;    break;
	case 2:  texContrib = tex3;    break;
	case 3:  texContrib = tex4;    break;
	case 4:  texContrib = tex5;    break;
	case 5:  texContrib = tex6;    break;
	case 6:  texContrib = tex7;    break;
	case 7:  texContrib = tex8;    break;
	case 8:  texContrib = tex9;    break;
	case 9:  texContrib = tex10;   break;
	case 10: texContrib = tex11;   break;
	case 11: texContrib = tex12;   break;
	case 12: texContrib = tex13;   break;
	case 13: texContrib = tex14;   break;
	case 14: texContrib = tex15;   break;
	case 15: texContrib = tex16;   break;
	default: texContrib = missing; break;
	}
    FragColor = colorContrib * texContrib;
}

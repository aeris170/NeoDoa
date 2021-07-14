#version 450 core

layout (location = 0) out vec4 FragColor;

flat in int v_color;

void main() {
    int rIntValue = (v_color / 256 / 256) % 256;
    int gIntValue = (v_color / 256) % 256;
    int bIntValue = (v_color) % 256;

    float r = rIntValue / 255.0f;
    float g = gIntValue / 255.0f;
    float b = bIntValue / 255.0f;

    FragColor = vec4(r, g, b, 1);

}

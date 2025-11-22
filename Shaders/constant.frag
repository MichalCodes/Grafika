#version 330 core

out vec4 out_Color;
uniform vec3 objectColor;

void main() {
    out_Color = vec4(objectColor, 1.0);
}

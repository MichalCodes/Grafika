#version 330 core
in vec3 vertColor;
out vec4 frag_colour;
void main() {
    frag_colour = vec4(vertColor, 1.0);
}
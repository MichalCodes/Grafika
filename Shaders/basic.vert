#version 330 core
layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 inColor;
uniform mat4 uMVP;
out vec3 vertColor;
void main() {
    gl_Position = uMVP * vec4(vp, 1.0);
    vertColor = inColor;
}
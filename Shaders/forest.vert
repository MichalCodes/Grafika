#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    // Pozice fragmentu ve světových souřadnicích
    FragPos = vec3(model * vec4(in_Position, 1.0));

    // Korektní transformace normály
    Normal = mat3(transpose(inverse(model))) * in_Normal;

    // Výpočet finální pozice na obrazovce
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

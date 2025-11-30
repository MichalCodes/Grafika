#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 uMVP; // Mělo by být uMVP nahrazeno výpočtem v kódu, ale necháme ho pro konzistenci
uniform float w = 1.0;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 TexCoord;

void main() {
    worldPos = vec3(model * vec4(aPos, 1.0));

    // Přesná transformace normal
    worldNormal = mat3(transpose(inverse(model))) * aNormal;
    
    TexCoord = aTex; 
    
    // Používáme worldPos pro výpočet gl_Position
    gl_Position = projection * view * vec4(worldPos, w);
}
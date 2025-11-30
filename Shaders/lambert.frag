#version 330 core

in vec3 worldNormal;
in vec3 worldPos;

out vec4 out_Color;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(lightPosition - worldPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.1 * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    out_Color = vec4(result, 1.0);
}

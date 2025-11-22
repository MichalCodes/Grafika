#version 330 core

in vec3 worldNormal;
in vec3 worldPos;

out vec4 out_Color;

uniform vec3 dirLightDirection;
uniform vec3 dirLightColor;

uniform vec3 viewPosition;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(worldNormal);

    vec3 lightDir = normalize(-dirLightDirection);

    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * dirLightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * dirLightColor;

    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPosition - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * dirLightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    out_Color = vec4(result, 1.0);
}
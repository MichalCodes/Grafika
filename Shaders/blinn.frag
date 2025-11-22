#version 330 core

in vec3 worldNormal;
in vec3 worldPos;

out vec4 out_Color;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPosition;

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(lightPosition - worldPos);
    vec3 viewDir = normalize(viewPosition - worldPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.1 * lightColor;

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = 0.4 * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    out_Color = vec4(result, 1.0);
}

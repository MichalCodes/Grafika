#version 330 core

in vec3 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 objectColor;

uniform vec3 viewPosition = vec3(0.0f, 0.0f, 5.0f);
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

const float materialAmbientStrength = 0.1f;
const float materialSpecularStrength = 0.5f;
const float materialShininess = 64.0f;

uniform bool hasTexture = false;
uniform sampler2D diffuseTex;

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(lightPosition - worldPos);
    vec3 viewDir  = normalize(viewPosition - worldPos);

    // původní sampling textury
    vec3 baseColor = hasTexture ? texture(diffuseTex, TexCoord).rgb : objectColor;

    // ambient
    vec3 ambient = materialAmbientStrength * lightColor * baseColor;

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    // ✔ opravený specular pouze když je planeta na světle
    float spec = 0.0;
    if (diff > 0.0) {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    }
    vec3 specular = materialSpecularStrength * lightColor * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

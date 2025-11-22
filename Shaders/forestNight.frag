#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutoff;
    float outerCutoff;
};

uniform PointLight pointLights[8];
uniform int numLights;
uniform SpotLight flashlight;
uniform vec3 viewPos;
uniform vec3 objectColor;

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

// ---- výpočet bodového světla ----
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                               light.linear * distance +
                               light.quadratic * (distance * distance));

    vec3 ambient  = 0.08 * light.color;
    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / max(epsilon, 0.001), 0.0, 1.0);

    vec3 ambient  = 0.05 * light.color;
    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;

    return (ambient + diffuse + specular) * intensity;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.02, 0.02, 0.02);

    result += CalcSpotLight(flashlight, norm, FragPos, viewDir);

    for (int i = 0; i < numLights; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result * objectColor, 1.0);
}

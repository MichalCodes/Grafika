#version 330 core

in vec3 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 viewPosition;

uniform vec3 objectColor;
const float materialAmbientStrength = 0.1f;
uniform float materialSpecularStrength = 0.5f;
uniform float materialShininess = 32.0f;       

uniform bool hasTexture = false;
uniform sampler2D diffuseTex;

uniform int lightingModel = 2; 

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 baseColor = hasTexture ? texture(diffuseTex, TexCoord).rgb : objectColor;

    vec3 ambient = materialAmbientStrength * lightColor * baseColor;

    vec3 lightDir = normalize(lightPosition - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    vec3 specular = vec3(0.0);
    float spec = 0.0;
    
    if (lightingModel != 0 && diff > 0.0) {
        vec3 viewDir = normalize(viewPosition - worldPos);

        if (lightingModel == 1) {
            vec3 reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        } else if (lightingModel == 2) {
            vec3 halfwayDir = normalize(lightDir + viewDir); 
            spec = pow(max(dot(norm, halfwayDir), 0.0), materialShininess);
        }
        
        specular = materialSpecularStrength * lightColor * spec;
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
#version 330 core

in vec3 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;

out vec4 FragColor;

// SVĚTELNÉ PARAMETRY
uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 viewPosition;

// MATERIÁL
uniform vec3 objectColor;
const float materialAmbientStrength = 0.1f;
uniform float materialSpecularStrength = 0.5f;
uniform float materialShininess = 32.0f;       

// TEXTURA
uniform bool hasTexture = false;
uniform sampler2D diffuseTex;

// PŘEPÍNAČ MODELŮ
// 0: Lambert
// 1: Phong
// 2: Blinn-Phong
uniform int lightingModel = 2; 

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 baseColor = hasTexture ? texture(diffuseTex, TexCoord).rgb : objectColor;

    // 1. Ambientní složka (společná)
    vec3 ambient = materialAmbientStrength * lightColor * baseColor;

    // 2. Difúzní složka (společná)
    vec3 lightDir = normalize(lightPosition - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    // 3. Spekulární složka (specifická pro model)
    vec3 specular = vec3(0.0);
    float spec = 0.0;
    
    // Spekulární výpočet se provádí pouze, pokud není Lambert (model 0)
    if (lightingModel != 0 && diff > 0.0) {
        vec3 viewDir = normalize(viewPosition - worldPos);

        if (lightingModel == 1) {
            // Phong
            vec3 reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        } else if (lightingModel == 2) {
            // Blinn-Phong
            vec3 halfwayDir = normalize(lightDir + viewDir); 
            spec = pow(max(dot(norm, halfwayDir), 0.0), materialShininess);
        }
        
        specular = materialSpecularStrength * lightColor * spec;
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
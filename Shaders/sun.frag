#version 330 core
out vec4 FragColor;
in vec2 TexCoord; 

uniform sampler2D sunTexture;
uniform vec3 objectColor;
// Použít stejnou konvenci jako v `phong.frag` - flag říká, zda je k dispozici textura
uniform bool hasTexture = false;

void main() {
    // Pokud máme texturu, načteme ji. Jinak použijeme `objectColor`.
    vec3 textureResult = vec3(0.0);
    if (hasTexture) {
        vec3 texColor = texture(sunTexture, TexCoord).rgb;
        textureResult = texColor * vec3(1.0, 0.85, 0.2);
    }

    vec3 finalColor = hasTexture ? textureResult : objectColor;

    // 3. Výpočet intenzity (pro point sprites, ponecháno pro kompatibilitu)
    float intensity = 1.0 - length(gl_PointCoord - vec2(0.5)) * 0.5;
    intensity = clamp(intensity, 0.0, 1.0);
    
    // 4. Výstup
    FragColor = vec4(finalColor * intensity, 1.0);
}
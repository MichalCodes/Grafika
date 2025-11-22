#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D sunTexture;

void main() {
    // Načteme texturu
    vec3 texColor = texture(sunTexture, TexCoord).rgb;

    // Přidáme intenzitu světla do středu (mírné glow)
    float intensity = 1.0 - length(gl_PointCoord - vec2(0.5)) * 0.5;
    intensity = clamp(intensity, 0.0, 1.0);

    // Kombinace barvy textury se žlutým glow efektem
    vec3 sunColor = texColor * vec3(1.0, 0.85, 0.2);
    FragColor = vec4(sunColor * intensity, 1.0);
}

#version 330 core
out vec4 FragColor;
in vec2 TexCoord; 

uniform sampler2D sunTexture;
uniform vec3 objectColor;
uniform bool hasTexture = false;

void main() {
    vec3 textureResult = vec3(0.0);
    if (hasTexture) {
        vec3 texColor = texture(sunTexture, TexCoord).rgb;
        textureResult = texColor * vec3(1.0, 0.85, 0.2);
    }

    vec3 finalColor = hasTexture ? textureResult : objectColor;

    float intensity = 1.0 - length(gl_PointCoord - vec2(0.5)) * 0.5;
    intensity = clamp(intensity, 0.0, 1.0);
    
    FragColor = vec4(finalColor * intensity, 1.0);
}
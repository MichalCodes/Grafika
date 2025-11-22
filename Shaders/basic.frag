#version 330 core

out vec4 FragColor;

uniform vec3 objectColor;
uniform sampler2D diffuseTex;
uniform int hasTexture;

in vec2 TexCoords;

void main()
{
    if (hasTexture == 1)
        FragColor = texture(diffuseTex, TexCoords);
    else
        FragColor = vec4(objectColor, 1.0);
}

#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main() {
    // Invert Y coordinate so textures appear upright (images were upside-down)
    TexCoords = vec3(position.x, -position.y, position.z);

    // Project the skybox and force depth to the far plane by setting z = w (xyww trick)
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = vec4(pos.xy, pos.w, pos.w);
}

#include "SkyCube.h"
#include "ProgramShader.h"
#include "Texture.h"
#include <stb/stb_image.h>
#include <iostream>

SkyCube::SkyCube() {
    setupCube();
}

SkyCube::~SkyCube() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (cubemapTexture != 0) glDeleteTextures(1, &cubemapTexture);
}

void SkyCube::setupCube() {
    // Skybox cube vertices (large cube centered at origin)
    float size = 1.0f;
    float vertices[] = {
        // Front
        -size,  size, -size,
        -size, -size, -size,
         size, -size, -size,
         size,  size, -size,
        // Back
        -size,  size,  size,
        -size, -size,  size,
         size, -size,  size,
         size,  size,  size,
        // Left
        -size,  size,  size,
        -size, -size,  size,
        -size, -size, -size,
        -size,  size, -size,
        // Right
         size,  size,  size,
         size, -size,  size,
         size, -size, -size,
         size,  size, -size,
        // Top
        -size,  size,  size,
        -size,  size, -size,
         size,  size, -size,
         size,  size,  size,
        // Bottom
        -size, -size,  size,
        -size, -size, -size,
         size, -size, -size,
         size, -size,  size,
    };
    
    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 6, 5, 6, 4, 7,       // Back
        8, 9, 10, 10, 11, 8,    // Left
        12, 14, 13, 14, 12, 15, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 22, 21, 22, 20, 23  // Bottom
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Keep EBO bound to VAO so we can draw elements later
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

bool SkyCube::loadCubemap(const std::string& posX, const std::string& negX,
                          const std::string& posY, const std::string& negY,
                          const std::string& posZ, const std::string& negZ) {
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
    // Order: +X, -X, +Y, -Y, +Z, -Z
    std::string faces[6] = { posX, negX, posY, negY, posZ, negZ };
    GLenum faceTargets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    
    for (int i = 0; i < 6; ++i) {
        int width, height, channels;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cerr << "Failed to load cubemap texture: " << faces[i] << std::endl;
            glDeleteTextures(1, &cubemapTexture);
            cubemapTexture = 0;
            return false;
        }
        
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(faceTargets[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return true;
}

void SkyCube::setShader(std::shared_ptr<ProgramShader> sdr) {
    shader = sdr;
}

void SkyCube::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!shader) {
        std::cerr << "SkyCube: Shader not set!" << std::endl;
        return;
    }
    
    if (cubemapTexture == 0) {
        std::cerr << "SkyCube: Cubemap texture not loaded!" << std::endl;
        return;
    }
    
    // Save current depth and cull state
    GLint depthFunc;
    GLboolean depthTestEnabled;
    GLboolean cullFaceEnabled;
    GLint cullFaceMode;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
    depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);

    // Render skybox: ensure it's drawn at far plane and not culled
    shader->use();
    glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(view));
    shader->setMat4("view", viewNoTranslate);
    shader->setMat4("projection", projection);
    shader->setUniform("skybox", 0);

    // Set depth function and disable depth writes so skybox is always behind
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    // Disable face culling to avoid winding/order issues for the cubemap
    if (cullFaceEnabled) glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Restore culling state
    if (cullFaceEnabled) glEnable(GL_CULL_FACE);
    glCullFace((GLenum)cullFaceMode);

    // Restore depth state
    glDepthMask(GL_TRUE);
    glDepthFunc(depthFunc);
    if (!depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }
}

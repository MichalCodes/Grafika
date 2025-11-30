#ifndef SKYCUBE_H
#define SKYCUBE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

class ProgramShader;

class SkyCube {
public:
    SkyCube();
    ~SkyCube();
    
    // Load cubemap textures (6 images for each face)
    bool loadCubemap(const std::string& posX, const std::string& negX,
                     const std::string& posY, const std::string& negY,
                     const std::string& posZ, const std::string& negZ);
    
    // Set shader for rendering
    void setShader(std::shared_ptr<ProgramShader> shader);
    
    // Render skybox (should be called with depth test enabled, depth func <= or far)
    void render(const glm::mat4& view, const glm::mat4& projection);
    
    GLuint getCubemapTexture() const { return cubemapTexture; }
    
private:
    GLuint cubemapTexture = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    std::shared_ptr<ProgramShader> shader;
    
    void setupCube();
};

#endif /* SKYCUBE_H */

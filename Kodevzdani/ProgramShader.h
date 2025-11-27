#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Shader.h"
#include "Camera.h"

using namespace std;

class ProgramShader {
public:
    ProgramShader(Camera* cam = nullptr);
    ProgramShader(const string& vertexPath, const string& fragmentPath);
    ~ProgramShader();

    bool loadFromFiles(const string& vertexPath, const string& fragmentPath);
    void use() const;
    void update();

    void setUniform(const string& name, const glm::vec3& value) const;
    void setMat4(const string& name, const glm::mat4& mat) const;
    void setUniform(const string& name, float value) const;
    void setUniform(const string& name, int value) const;
    void setUniform(const string& name, bool value) const;
    void setTexture(const string& name, GLuint textureID, int unit) const;

private:
    bool linkProgram(const Shader& vertexShader, const Shader& fragmentShader);
    GLuint programID;
    Camera* camera;
};

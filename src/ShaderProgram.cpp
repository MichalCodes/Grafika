#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;

ShaderProgram::ShaderProgram(Camera* cam) : camera(cam) {
    programID = glCreateProgram();
}

ShaderProgram::ShaderProgram(const string& vertexPath, const string& fragmentPath)
    : camera(nullptr)
{
    Shader vertexShader(GL_VERTEX_SHADER);
    Shader fragmentShader(GL_FRAGMENT_SHADER);

    if (!vertexShader.loadFromFile(vertexPath)) {
        cerr << "Chyba při načítání vertex shaderu: " << vertexPath << endl;
        return;
    }
    if (!fragmentShader.loadFromFile(fragmentPath)) {
        cerr << "Chyba při načítání fragment shaderu: " << fragmentPath << endl;
        return;
    }

    programID = glCreateProgram();
    vertexShader.attachTo(programID);
    fragmentShader.attachTo(programID);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        cerr << "Linking shader programu selhalo:\n" << infoLog << endl;
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programID);
}

bool ShaderProgram::linkProgram(const Shader& vertexShader, const Shader& fragmentShader) {
    vertexShader.attachTo(programID);
    fragmentShader.attachTo(programID);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        cerr << "Linking shader programu selhalo:\n" << infoLog << endl;
        return false;
    }
    return true;
}

void ShaderProgram::use() const {
    glUseProgram(programID);
}

void ShaderProgram::update() {
    if (camera) {
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix(1000.0f / 800.0f);
        setMat4("view", view);
        setMat4("projection", projection);
    }
}

void ShaderProgram::setMat4(const string& name, const glm::mat4& mat) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniform(const string& name, float value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1f(loc, value);
}

void ShaderProgram::setUniform(const string& name, int value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, value);
}

void ShaderProgram::setUniform(const string& name, bool value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, value ? 1 : 0);
}

void ShaderProgram::setTexture(const string& name, GLuint textureID, int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, unit);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& mat) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& vec) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform3fv(loc, 1, glm::value_ptr(vec));
}




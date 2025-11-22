#include "ProgramShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;

ProgramShader::ProgramShader(Camera* cam) : camera(cam) {
    programID = glCreateProgram();
}

ProgramShader::ProgramShader(const string& vertexPath, const string& fragmentPath)
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

ProgramShader::~ProgramShader() {
    glDeleteProgram(programID);
}

bool ProgramShader::loadFromFiles(const string& vertexPath, const string& fragmentPath) {
    Shader vertexShader(GL_VERTEX_SHADER);
    Shader fragmentShader(GL_FRAGMENT_SHADER);

    if (!vertexShader.loadFromFile(vertexPath)) return false;
    if (!fragmentShader.loadFromFile(fragmentPath)) return false;

    return linkProgram(vertexShader, fragmentShader);
}

bool ProgramShader::linkProgram(const Shader& vertexShader, const Shader& fragmentShader) {
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

void ProgramShader::use() const {
    glUseProgram(programID);
}

void ProgramShader::update() {
    if (camera) {
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix(1000.0f / 800.0f);
        setMat4("view", view);
        setMat4("projection", projection);
    }
}

void ProgramShader::setUniform(const string& name, const glm::vec3& value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform3fv(loc, 1, &value[0]);
}

void ProgramShader::setMat4(const string& name, const glm::mat4& mat) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ProgramShader::setUniform(const string& name, float value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1f(loc, value);
}

void ProgramShader::setUniform(const string& name, int value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, value);
}

void ProgramShader::setUniform(const string& name, bool value) const {
    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, value ? 1 : 0);
}

void ProgramShader::setTexture(const string& name, GLuint textureID, int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint loc = glGetUniformLocation(programID, name.c_str());
    if (loc != -1)
        glUniform1i(loc, unit);
}

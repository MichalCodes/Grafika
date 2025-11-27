#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(GLenum type) : shaderType(type) {
    shaderID = glCreateShader(type);
}

Shader::~Shader() {
    glDeleteShader(shaderID);
}

bool Shader::loadFromFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Nelze otevřít shader soubor: " << path << endl;
        return false;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return compile(buffer.str());
}

bool Shader::compile(const string& source) {
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Shader kompilace selhala:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

void Shader::attachTo(GLuint programID) const {
    glAttachShader(programID, shaderID);
}
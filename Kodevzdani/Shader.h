#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
using namespace std;

class Shader {
public:
    Shader(GLenum type);
    ~Shader();

    bool loadFromFile(const string& path);
    void attachTo(GLuint programID) const;

private:
    GLuint shaderID;
    GLenum shaderType;
    bool compile(const string& source);
};

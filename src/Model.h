#pragma once
#include <GL/glew.h>
#include <string>
#include <memory>

class Model {
private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    bool hasTexcoords = false;
    int vertexCount = 0;

public:
    Model(); 
    Model(const float* vertices, int count);
    Model(const float* vertices, int count, bool hasTex);
    ~Model();

    void draw() const;

    static std::shared_ptr<Model> loadOBJ(const std::string &filename);
};

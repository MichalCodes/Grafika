#pragma once
#include <string>
#include <GL/glew.h>
#include <memory>
#include "Texture.h"

class Texture {
public:
    Texture(const std::string& path, bool flipVertically = true);
    ~Texture();

    void bind(GLuint unit = 0) const;
    void unbind() const;
    void setTexture(GLuint unit) const;
    void loadFromFile(const std::string& path, bool flipVertically = true);
    GLuint getID() const { return textureID; }

private:
    GLuint textureID;
};
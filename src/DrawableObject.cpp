#include "DrawableObject.h"

void DrawableObject::draw(const glm::mat4& mvp) const {
    shader->use();
    shader->setMat4("model", transform->getMat());
    shader->setMat4("uMVP", mvp);
    shader->setUniform("objectColor", color);

    if (texture) {
        shader->setUniform("hasTexture", 1);
        texture->bind(0);
        shader->setUniform("diffuseTex", 0);
    } else {
        shader->setUniform("hasTexture", 0);
    }

    model->draw();

    if (texture) texture->unbind();
}

void DrawableObject::drawForPicking(const glm::mat4& mvp) const {
    shader->use();
    shader->setMat4("model", transform->getMat());
    shader->setMat4("uMVP", mvp);
    model->draw();
}

void DrawableObject::setColor(const glm::vec3& color) {
    this->color = color;
}
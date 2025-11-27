#pragma once
#include "TransformationComponent.h"
#include <glm/gtc/matrix_transform.hpp>

class Translate : public TransformationComponent {
public:
    Translate(const glm::vec3& t) : translation(t) {}
    void apply(glm::mat4& mat) const override {
        mat = glm::translate(mat, translation);
    }

private:
    glm::vec3 translation;
};

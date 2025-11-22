#pragma once
#include "TransformationComponent.h"
#include <glm/gtc/matrix_transform.hpp>

class Scale : public TransformationComponent {
public:
    Scale(const glm::vec3& s) : scale(s) {}
    void apply(glm::mat4& mat) const override {
        mat = glm::scale(mat, scale);
    }

private:
    glm::vec3 scale;
};

#pragma once
#include "TransformationComponent.h"
#include <glm/gtc/matrix_transform.hpp>

class Rotate : public TransformationComponent {
public:
    Rotate(float angleRad, const glm::vec3& axis)
        : angle(angleRad), axis(axis) {}

    void apply(glm::mat4& mat) const override {
        mat = glm::rotate(mat, angle, axis);
    }

private:
    float angle;
    glm::vec3 axis;
};

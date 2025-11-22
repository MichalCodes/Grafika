#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

Transformation::Transformation() {}

void Transformation::add(std::shared_ptr<TransformationComponent> component) {
    components.push_back(component);
}

void Transformation::clear() {
    components.clear();
}

void Transformation::apply(glm::mat4& mat) const {
    for (const auto& comp : components) {
        comp->apply(mat);
    }
}

glm::mat4 Transformation::getMat() const {
    glm::mat4 mat(1.0f);
    apply(mat);
    return mat;
}

const float* Transformation::data() const {
    static glm::mat4 tmp = getMat();
    return &tmp[0][0];
}

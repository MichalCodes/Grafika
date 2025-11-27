#pragma once
#include <glm/glm.hpp>

class TransformationComponent {
public:
    virtual ~TransformationComponent() = default;
    virtual void apply(glm::mat4& mat) const = 0;
};

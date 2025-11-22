#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "TransformationComponent.h"

using namespace std;

class Transformation : public TransformationComponent {
public:
    Transformation();

    void add(shared_ptr<TransformationComponent> component);
    void clear();

    void apply(glm::mat4& mat) const override;
    glm::mat4 getMat() const;
    const float* data() const;

private:
    vector<shared_ptr<TransformationComponent>> components;
};

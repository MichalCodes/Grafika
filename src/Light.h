#pragma once
#include <glm/glm.hpp>
#include "ProgramShader.h"
#include <memory>

class Light {
public:
    Light();

    Light(const glm::vec3& position, const glm::vec3& color,
          float intensity = 1.0f,
          float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

    static Light Directional(const glm::vec3& direction,
                             const glm::vec3& color,
                             float intensity = 1.0f);

    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;
    glm::vec3 getColor() const;

    float getIntensity() const;
    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;

    void setPosition(const glm::vec3& p);
    void setDirection(const glm::vec3& d);
    void setColor(const glm::vec3& c);
    void setIntensity(float i);
    void setAttenuation(float constant, float linear, float quadratic);

    void applyToShader(const std::shared_ptr<ProgramShader>& shader) const;

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;

    bool directional;
};

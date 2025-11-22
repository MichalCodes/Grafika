#include "Light.h"

Light::Light(): position(0,0,0),direction(0,-1,0),color(1,1,1),intensity(1.0f),
      constant(1.0f), linear(0.09f), quadratic(0.032f),directional(false)
{}

Light::Light(const glm::vec3& pos, const glm::vec3& col,float i, float c, float l, float q) : position(pos),
      direction(0,-1,0), color(col),intensity(i),constant(c), linear(l), quadratic(q),directional(false)
{}

Light Light::Directional(const glm::vec3& dir, const glm::vec3& col,float i){
    Light l;
    l.direction = glm::normalize(dir);
    l.color = col;
    l.intensity = i;
    l.directional = true;
    return l;
}

glm::vec3 Light::getPosition() const { return position; }
glm::vec3 Light::getDirection() const { return direction; }
glm::vec3 Light::getColor() const { return color; }

float Light::getIntensity() const { return intensity; }
float Light::getConstant() const { return constant; }
float Light::getLinear() const { return linear; }
float Light::getQuadratic() const { return quadratic; }

void Light::setPosition(const glm::vec3& p) { position = p; }
void Light::setDirection(const glm::vec3& d) {
    direction = glm::normalize(d);
    directional = true;
}

void Light::setColor(const glm::vec3& c) { color = c; }
void Light::setIntensity(float i) { intensity = i; }

void Light::setAttenuation(float c, float l, float q) {
    constant = c; linear = l; quadratic = q;
}

void Light::applyToShader(const std::shared_ptr<ProgramShader>& shader) const
{
    shader->use();
    shader->setUniform("light.color", color * intensity);

    if (directional)
    {
        shader->setUniform("light.isDirectional", 1);
        shader->setUniform("light.direction", direction);
    }
    else
    {
        shader->setUniform("light.isDirectional", 0);
        shader->setUniform("light.position", position);
        shader->setUniform("light.constant", constant);
        shader->setUniform("light.linear", linear);
        shader->setUniform("light.quadratic", quadratic);
    }
}

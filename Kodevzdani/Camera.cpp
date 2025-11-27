#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include "Observer.h"

Camera::Camera(glm::vec3 pos)
    : position(pos),
      worldUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      fov(45.0f),
      speed(2.5f),
      sensitivity(0.1f)
{
    updateCameraVectors();
}

void Camera::processKeyboard(float deltaForward, float deltaRight, float deltaTime) {
    float velocity = speed * deltaTime;
    if (deltaForward != 0.0f)
        position += front * velocity * deltaForward;
    if (deltaRight != 0.0f)
        position += right * velocity * deltaRight;
    notify();
}
void Camera::processMouse(float xOffset, float yOffset) {
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    yaw   += xOffset;
    pitch += yOffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
    notify();
}
// Matice
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}
glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}
// Observer pattern
void Camera::addObserver(Observer* obs) {
    observers.push_back(obs);
}
void Camera::removeObserver(Observer* obs) {
    observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
}
void Camera::notify() {
    for (auto obs : observers)
        obs->update();
}
void Camera::updateCameraVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
void Camera::reset() {
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    updateCameraVectors();
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getFront() const {
    return front;
}
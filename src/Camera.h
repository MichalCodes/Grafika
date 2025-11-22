#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Subject.h"
class Camera : Subject {
public:
    Camera(glm::vec3 pos);
    void processKeyboard(float deltaForward, float deltaRight, float deltaTime);
    void processMouse(float xOffset, float yOffset);
    void reset();
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    void addObserver(Observer* obs);
    void removeObserver(Observer* obs);
    void notify();
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    void setFront(const glm::vec3& newFront) {
        front = glm::normalize(newFront);
        updateCameraVectors();
    }
    void setPosition(const glm::vec3& newPos) {
        position = newPos;
    }
    void updateCameraVectors();
    glm::vec3 position;
    glm::vec3 front;
private:
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw; 
    float pitch;
    float fov;
    float speed = 2.5f;
    float sensitivity = 0.1f;
    std::vector<Observer*> observers;
};
#include "EventController.h"
#include <iostream>

static EventController* currentController = nullptr;

EventController::EventController(GLFWwindow* window,std::shared_ptr<Camera> camera,Scene& sceneManager)
    : window(window), camera(camera), sceneManager(sceneManager)
{ glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

void EventController::processInput(float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) sceneManager.setActiveScene(0);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) sceneManager.setActiveScene(1);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) sceneManager.setActiveScene(2);
    if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { sceneManager.setActiveScene(3);}
    if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) { sceneManager.setActiveScene(4);}
    if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) { sceneManager.setActiveScene(5);}
    if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) { sceneManager.setActiveScene(6);}
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && camera) camera->reset();

    static bool eKeyWasPressed = false;

    static bool fKeyWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!eKeyWasPressed) {
            eKeyWasPressed = true;
            isCursorEnabled = !isCursorEnabled;
            
            if (isCursorEnabled) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            firstMouse = true; 
        }
    } else {
        eKeyWasPressed = false;
    }
    
    // Toggle flashlight in forest scene (press F)
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!fKeyWasPressed) {
            fKeyWasPressed = true;
            sceneManager.toggleFlashlight();
        }
    } else {
        fKeyWasPressed = false;
    }
    
    float deltaForward = 0.0f;
    float deltaRight = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) deltaForward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) deltaForward -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) deltaRight += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) deltaRight -= 1.0f;
    if (camera) camera->processKeyboard(deltaForward, deltaRight, deltaTime);
}

void EventController::processMouse() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);
    lastX = xpos;
    lastY = ypos;
    if (camera) camera->processMouse(xoffset, yoffset);
}

void EventController::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentController) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glm::mat4 projection = currentController->camera->getProjectionMatrix((float)width / (float)height);
        glm::mat4 view = currentController->camera->getViewMatrix();
        currentController->sceneManager.handleMouseClick((float)xpos, (float)ypos, view, projection, width, height);
    }
}

void EventController::setGlobalController(EventController* controller) {
    currentController = controller;
}
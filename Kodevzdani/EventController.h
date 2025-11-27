#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Camera.h"
#include "Scene.h"
using namespace std;
class EventController {
public:
    EventController(GLFWwindow* window, shared_ptr<Camera> camera, Scene& sceneManager);
    void processInput(float deltaTime);
    void processMouse();
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void setGlobalController(EventController* controller);
private:
    GLFWwindow* window;
    shared_ptr<Camera> camera;
    Scene& sceneManager;
    double lastX = 500.0;
    double lastY = 400.0;
    bool firstMouse = true;
    bool isCursorEnabled = false;
};
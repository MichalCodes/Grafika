#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "EventController.h"
#include <memory>
#include <vector>
#include "Model.h"
#include "ProgramShader.h"
#include "Transformation.h"
#include "DrawableObject.h"
#include "Camera.h"
#include "Scene.h"
using namespace std;
class Application {
public:
    Application();
    ~Application();
    void run();
private:
    GLFWwindow* window;
    shared_ptr<ProgramShader> shader;
    shared_ptr<ProgramShader> shaderForest;
    Scene sceneManager;
    shared_ptr<DrawableObject> triangleObject;
    shared_ptr<Camera> camera;
    unique_ptr<EventController> inputController;
};
#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;

Application::Application() {

    if (!glfwInit()) {
        cerr << "Failed to init GLFW\n";
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1000, 800, "Modern OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW\n";
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);

    camera = make_shared<Camera>(glm::vec3(0.0f, 0.2f, 0.0f));
    camera->front = glm::normalize(glm::vec3(0.5f, 0.2f, 0.5f) - camera->position);
    camera->updateCameraVectors();
    shader = make_shared<ProgramShader>(camera.get());

    auto shaderLight = make_shared<ProgramShader>(camera.get());
    if (!shaderLight->loadFromFiles("Shaders/phong.vert", "Shaders/phong.frag")) { // nacitam phonga
        cerr << "Nepodařilo se načíst Phong shader!\n";
        exit(EXIT_FAILURE);
    }
    auto shaderSun = make_shared<ProgramShader>(camera.get());
    if (!shaderSun->loadFromFiles("Shaders/sun.vert", "Shaders/sun.frag")) { // slunce
        cerr << "Nepodařilo se načíst Sun shader!\n";
        exit(EXIT_FAILURE);
    }
    auto shaderForest = make_shared<ProgramShader>(camera.get());
    if (!shaderForest->loadFromFiles("Shaders/forest.vert", "Shaders/forest.frag")) { // slunce
        cerr << "Nepodařilo se načíst Sun shader!\n";
        exit(EXIT_FAILURE);
    }

    auto blinn = make_shared<ProgramShader>(camera.get());
    if (!blinn->loadFromFiles("Shaders/blinn.vert", "Shaders/blinn.frag")) { // slunce
        cerr << "Nepodařilo se načíst Sun shader!\n";
        exit(EXIT_FAILURE);
    }

    auto shaderSkybox = make_shared<ProgramShader>(camera.get());
    if (!shaderSkybox->loadFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag")) {
        cerr << "Nepodařilo se načíst Skybox shader!\n";
        exit(EXIT_FAILURE);
    }

    auto scene1 = sceneManager.initializeScene1(shaderLight);
    auto scene2 = sceneManager.initializeScene2(shaderLight);
    auto scene3 = sceneManager.initializeScene3(shaderLight);
    auto scene4 = sceneManager.initializeScene4(shaderLight, shaderSun);
    auto scene5 = sceneManager.initializeScene5(shaderForest);
    auto scene6 = sceneManager.initializeScene6(blinn);
    
    sceneManager.initializeSkyCube(shaderSkybox);

    sceneManager.addScene(scene1);
    sceneManager.addScene(scene2);
    sceneManager.addScene(scene3);
    sceneManager.addScene(scene4); 
    sceneManager.addScene(scene5);
    sceneManager.addScene(scene6);

    sceneManager.setActiveScene(3);
    inputController = make_unique<EventController>(window, camera, sceneManager);

    glfwSetMouseButtonCallback(window, EventController::mouseButtonCallback);
    EventController::setGlobalController(inputController.get());
}

void Application::run() {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        inputController->processInput(deltaTime);
        inputController->processMouse();

        float time = glfwGetTime();
        sceneManager.update(time, shaderForest, camera);
        shader->update();

        glm::mat4 projection = camera->getProjectionMatrix(1000.0f / 800.0f);
        glm::mat4 view = camera->getViewMatrix();

        sceneManager.draw(projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Application::~Application() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

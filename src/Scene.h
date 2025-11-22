#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Light.h"
#include "ProgramShader.h"
#include "TransformationComponent.h"
#include "Translate.h"
#include "Scale.h"
#include "Rotate.h"
#include "DrawableObject.h"
#include "Camera.h"
#include "Texture.h"
#include "CustomTransform.hpp"
using namespace std;
class DrawableObject;
class ProgramShader;
class Scene {
public:
    Scene();
    void addScene(const vector<shared_ptr<DrawableObject>>& sceneObjects);
    void setActiveScene(int index);
    int getActiveSceneIndex() const { return activeSceneIndex; }
    void draw(const glm::mat4& projection, const glm::mat4& view) const;
    void update(float time, shared_ptr<ProgramShader> shader, shared_ptr<Camera> Camera);
    void handleMouseClick(float mouseX, float mouseY, const glm::mat4& view, const glm::mat4& projection, int viewportWidth, int viewportHeight);
    void drawForPicking(const glm::mat4& projection, const glm::mat4& view) const;
    void setSelectedObject(unsigned int objectID);
    vector<shared_ptr<DrawableObject>> initializeScene1(shared_ptr<ProgramShader> shader);
    vector<shared_ptr<DrawableObject>> initializeScene2(shared_ptr<ProgramShader> shader);
    vector<shared_ptr<DrawableObject>> initializeScene3(shared_ptr<ProgramShader> shader);
    vector<shared_ptr<DrawableObject>> initializeScene4(shared_ptr<ProgramShader> shader, shared_ptr<ProgramShader> sunShader);
private:
    vector<vector<shared_ptr<DrawableObject>>> scenes;
    int activeSceneIndex;
    std::vector<Light> pointLights;
    shared_ptr<DrawableObject> cubeInScene6;
    shared_ptr<DrawableObject> formulaInScene6;
    shared_ptr<DrawableObject> selectedObject;
    bool isObjectSelected = false;
    shared_ptr<DrawableObject> solarSun;
    shared_ptr<DrawableObject> solarEarth;
    shared_ptr<DrawableObject> solarMoon;
    shared_ptr<DrawableObject> triangleObject;
    shared_ptr<DrawableObject> suziPhongNorm;
    shared_ptr<Texture> planeTexture;
    shared_ptr<DrawableObject> skyboxObject;
    struct Flashlight {
        glm::vec3 position;
        glm::vec3 direction;
        float cutoff = glm::cos(glm::radians(12.5f));
        float outerCutoff = glm::cos(glm::radians(17.5f));
    } flashlight;
    struct Firefly {
        glm::vec3 position;
        glm::vec3 color;
    };
    vector<Firefly> fireflies;
    shared_ptr<ProgramShader> forestShader;
    std::vector<std::shared_ptr<DrawableObject>> lightMarkers;
    std::vector<glm::vec3> lightBasePositions;
    shared_ptr<ProgramShader> skyboxShader;
    shared_ptr<DrawableObject> solarMars;
    shared_ptr<DrawableObject> solarJupiter;
    shared_ptr<DrawableObject> solarSaturn;
    shared_ptr<DrawableObject> solarUranus;
    shared_ptr<DrawableObject> solarNeptune;
    shared_ptr<DrawableObject> solarMercury;
    shared_ptr<DrawableObject> solarVenus;
};
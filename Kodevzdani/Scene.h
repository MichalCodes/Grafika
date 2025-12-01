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
#include "SkyCube.h"
#include <algorithm>
using namespace std;
class DrawableObject;
class ProgramShader;
class SkyCube;
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
    vector<shared_ptr<DrawableObject>> initializeScene5(shared_ptr<ProgramShader> shader);
    vector<shared_ptr<DrawableObject>> initializeScene6(shared_ptr<ProgramShader> pbr_like_shader, shared_ptr<ProgramShader> lambertShader);
    vector<shared_ptr<DrawableObject>> initializeScene7(shared_ptr<ProgramShader> shader);
    void initializeSkyCube(shared_ptr<ProgramShader> skyboxShader);
    void toggleFlashlight();
private:
    vector<vector<shared_ptr<DrawableObject>>> scenes;
    int activeSceneIndex;
    shared_ptr<DrawableObject> formulaInScene6;
    shared_ptr<DrawableObject> cubeInScene6;
    shared_ptr<DrawableObject> selectedObject;
    bool isObjectSelected = false;
    shared_ptr<DrawableObject> solarSun;
    shared_ptr<DrawableObject> solarEarth;
    shared_ptr<DrawableObject> solarMoon;
    shared_ptr<DrawableObject> suziPhongNorm;
    shared_ptr<Texture> planeTexture;
    shared_ptr<DrawableObject> skyboxObject;
    std::vector<std::shared_ptr<DrawableObject>> lightMarkers;
    std::vector<glm::vec3> lightBasePositions;
    shared_ptr<DrawableObject> solarMars;
    shared_ptr<DrawableObject> solarJupiter;
    shared_ptr<DrawableObject> solarSaturn;
    shared_ptr<DrawableObject> solarUranus;
    shared_ptr<DrawableObject> solarNeptune;
    shared_ptr<DrawableObject> solarMercury;
    shared_ptr<DrawableObject> solarVenus;
    shared_ptr<DrawableObject> triangleObject;
    struct MoleCube {
        shared_ptr<DrawableObject> obj;
        float baseY;
        float currentY;
        float baseX;
        float baseZ;
        bool emerging;
        float speed;
        float stayTimer;
        bool isHit = false;

        MoleCube() = default;
        ~MoleCube() = default;
    };

    struct FormulaObject {
        shared_ptr<DrawableObject> obj;
        glm::vec3 startPos;
        glm::vec3 endPos;
        float currentT;
        // control points for cubic Bezier (on plane)
        glm::vec3 cp1;
        glm::vec3 cp2;
        float speed = 0.25f; // t per second
        bool isActive = false;
    };

    FormulaObject formula;
    vector<MoleCube> moleCubes;
    float moleTimer = 0.0f;
    float lastUpdateTime = 0.0f;
    int currentScore = 0;

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
    std::vector<Light> pointLights;
    shared_ptr<ProgramShader> sunShader;
    shared_ptr<ProgramShader> universalShader;
    shared_ptr<SkyCube> skyCube;
    bool flashlightEnabled = true;
    glm::vec3 sceneLightPos = glm::vec3(2.0f, 0.0f, 10.0f);
    glm::vec3 scene3DirectionalPos = glm::vec3(0.0f, 10.0f, 0.0f);
};

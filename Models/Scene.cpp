#include "Scene.h"
#include "../Models/sphere.h"
#include "../Models/plain.h"
#include "../Models/tree.h"
#include "../Models/bushes.h"
#include "../Models/suzi_flat.h"
#include "../Models/suzi_smooth.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

using namespace std;

Scene::Scene() : activeSceneIndex(0) {}

void Scene::addScene(const vector<shared_ptr<DrawableObject>>& sceneObjects) {
    scenes.push_back(sceneObjects);
}

void Scene::setActiveScene(int index) {
    if (index >= 0 && index < static_cast<int>(scenes.size())) {
        activeSceneIndex = index;
    } else {
        cerr << "Scene index out of range: " << index << endl;
    }
}

void Scene::draw(const glm::mat4& projection, const glm::mat4& view) const {
    if (activeSceneIndex < 0 || activeSceneIndex >= (int)scenes.size())
        return;

    const auto& activeScene = scenes[activeSceneIndex];
    for (const auto& obj : activeScene) {
        glm::mat4 model = obj->getTransform()->getMat();
        glm::mat4 mvp = projection * view * model;

        obj->getShader()->use();
        obj->getShader()->setMat4("model", model);
        obj->getShader()->setMat4("view", view);
        obj->getShader()->setMat4("projection", projection);

        obj->draw(mvp);
    }
}

void Scene::update(float time, shared_ptr<ProgramShader> shader, shared_ptr<Camera> camera) {
    if (activeSceneIndex == 0 && triangleObject) {
        auto composite = make_shared<Transformation>();
        composite->add(make_shared<Translate>(glm::vec3(0.0f, -0.2f, 0.0f)));
        composite->add(make_shared<Rotate>(time, glm::vec3(0.0f, 0.0f, 1.0f)));
        triangleObject->setTransformation(composite);
    }
    else if (activeSceneIndex == 1) {
        shader->use();

        glm::vec3 camPos = camera->getPosition();
        shader->setUniform("viewPos", camPos);

        shader->setUniform("flashlight.position", camPos);
        shader->setUniform("flashlight.direction", camera->getFront());
        shader->setUniform("flashlight.color", glm::vec3(1.0f, 1.0f, 0.95f));
        shader->setUniform("flashlight.cutoff", glm::cos(glm::radians(12.5f)));
        shader->setUniform("flashlight.outerCutoff", glm::cos(glm::radians(17.5f)));

        if (!lightBasePositions.empty() && !pointLights.empty()) {
            for (size_t i = 0; i < pointLights.size() && i < lightBasePositions.size(); ++i) {
                        float baseY = lightBasePositions[i].y;
                        float verticalAmplitude = 0.12f;
                        float orbitY = baseY - verticalAmplitude;

                        float speed = 0.25f + static_cast<float>(i) * 0.05f;
                        float radius = 0.6f;
                        float phase = time * speed + static_cast<float>(i) * 1.2f;

                        float x = lightBasePositions[i].x + cos(phase) * radius;
                        float z = lightBasePositions[i].z + sin(phase) * radius;
                        glm::vec3 newPos = glm::vec3(x, orbitY, z);

                        float pulse = 0.5f + 0.5f * (0.5f + 0.5f * sin(phase * 2.5f));

                        pointLights[i].setPosition(newPos);
                        pointLights[i].setIntensity(pulse);

                        if (i < lightMarkers.size() && lightMarkers[i]) {
                            auto t = make_shared<Transformation>();
                            t->add(make_shared<Translate>(newPos));
                            float scale = 0.003f + 0.004f * ((pulse - 0.5f) / 0.5f);
                            t->add(make_shared<Scale>(glm::vec3(scale)));
                            lightMarkers[i]->setTransformation(t);
                        }
            }
        }

        if (!pointLights.empty()) {
            shader->setUniform("numLights", static_cast<int>(pointLights.size()));
            for (size_t i = 0; i < pointLights.size(); ++i) {
                const auto &pl = pointLights[i];
                std::string idx = "pointLights[" + std::to_string(i) + "]";
                shader->setUniform(idx + ".position", pl.getPosition());
                shader->setUniform(idx + ".color", pl.getColor() * pl.getIntensity());
                shader->setUniform(idx + ".constant", pl.getConstant());
                shader->setUniform(idx + ".linear", pl.getLinear());
                shader->setUniform(idx + ".quadratic", pl.getQuadratic());
            }
        } else {
            if (fireflies.empty()) {
                fireflies.push_back({ glm::vec3(-2.0f, 0.4f, -2.0f), glm::vec3(1.0f, 0.7f, 0.5f) });
                fireflies.push_back({ glm::vec3( 2.0f, 0.6f, -1.0f), glm::vec3(0.5f, 0.7f, 1.0f) });
                fireflies.push_back({ glm::vec3(-1.5f, 0.5f,  2.0f), glm::vec3(0.8f, 1.0f, 0.6f) });
                fireflies.push_back({ glm::vec3( 1.2f, 0.5f,  1.8f), glm::vec3(1.0f, 0.6f, 0.8f) });
            }
            shader->setUniform("numLights", static_cast<int>(fireflies.size()));
            for (size_t i = 0; i < fireflies.size(); ++i) {
                const auto& f = fireflies[i];
                std::string idx = "pointLights[" + std::to_string(i) + "]";
                shader->setUniform(idx + ".position", f.position);
                shader->setUniform(idx + ".color", f.color);
                shader->setUniform(idx + ".constant", 1.0f);
                shader->setUniform(idx + ".linear", 0.045f);
                shader->setUniform(idx + ".quadratic", 0.0075f);
            }
        }
        shader->setUniform("objectColor", glm::vec3(0.6f, 0.8f, 0.6f));
    }
    else if (activeSceneIndex == 3) {
        if (!solarEarth || !solarMoon) return;

        float earthOrbitRadius = 3.0f;
        float earthSpeed = 1.0f;
        glm::vec3 earthPos(
            earthOrbitRadius * cos(time * earthSpeed),
            earthOrbitRadius * sin(time * earthSpeed),
            0.0f
        );

        auto earthTrans = make_shared<Transformation>();
        earthTrans->add(make_shared<Translate>(earthPos));
        earthTrans->add(make_shared<Scale>(glm::vec3(0.2f)));
        solarEarth->setTransformation(earthTrans);

        float moonOrbitRadius = 0.6f;
        float moonSpeed = 3.0f;
        glm::vec3 moonPos(
            earthPos.x + moonOrbitRadius * cos(time * moonSpeed),
            earthPos.y + moonOrbitRadius * sin(time * moonSpeed),
            0.0f
        );

        auto moonTrans = make_shared<Transformation>();
        moonTrans->add(make_shared<Translate>(moonPos));
        moonTrans->add(make_shared<Scale>(glm::vec3(0.1f)));
        solarMoon->setTransformation(moonTrans);
    }
    else if (activeSceneIndex == 4 && suziPhongNorm) {
        auto rot = make_shared<Transformation>();
        rot->add(make_shared<Translate>(glm::vec3(2.2f, -0.5f, 0.0f)));
        rot->add(make_shared<Rotate>(time * 0.8f, glm::vec3(0.0f, 1.0f, 0.0f)));
        rot->add(make_shared<Scale>(glm::vec3(0.5f)));
        suziPhongNorm->setTransformation(rot);
    }
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene1(shared_ptr<ProgramShader> shader){
    float triangleVertices[] = {
         0.0f,  0.4f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.433f, -0.25f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.433f, -0.25f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    auto triangleModel = make_shared<Model>(triangleVertices, 3);
    triangleObject = make_shared<DrawableObject>(triangleModel, shader, make_shared<Transformation>());
    return {triangleObject};
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene2(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene2;

    float floorVertices[] = {
        -5.0f, 0.0f, -5.0f,  0.0f, 1.0f, 0.0f,
         5.0f, 0.0f, -5.0f,  0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f, -5.0f,  0.0f, 1.0f, 0.0f,
         5.0f, 0.0f,  5.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, 0.0f,  5.0f,  0.0f, 1.0f, 0.0f
    };
    auto floorModel = make_shared<Model>(floorVertices, 6);
    scene2.push_back(make_shared<DrawableObject>(floorModel, shader, make_shared<Transformation>()));

    int treeVertexCount = sizeof(tree) / sizeof(tree[0]) / 6;
    auto treeModel = make_shared<Model>(tree, treeVertexCount);
    std::default_random_engine eng;
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
    for (int i = 0; i < 50; ++i) {
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(glm::vec3(dist(eng), 0.0f, dist(eng))));
        t->add(make_shared<Scale>(glm::vec3(0.05f)));
        scene2.push_back(make_shared<DrawableObject>(treeModel, shader, t));
    }

    int bushVertexCount = sizeof(bushes) / sizeof(bushes[0]) / 6;
    auto bushModel = make_shared<Model>(bushes, bushVertexCount);
    for (int i = 0; i < 50; ++i) {
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(glm::vec3(dist(eng), 0.0f, dist(eng))));
        t->add(make_shared<Scale>(glm::vec3(0.5f)));
        scene2.push_back(make_shared<DrawableObject>(bushModel, shader, t));
    }

    pointLights.clear();
    lightBasePositions.clear();
    lightMarkers.clear();

    lightBasePositions.push_back(glm::vec3(-1.0f, 0.35f, -1.2f));
    lightBasePositions.push_back(glm::vec3(0.6f,  0.30f,  0.4f));
    lightBasePositions.push_back(glm::vec3(1.2f,  0.40f,  1.0f));

    for (const auto &pos : lightBasePositions) {
        Light pl(pos, glm::vec3(1.0f, 0.9f, 0.7f), 0.9f);
        pl.setAttenuation(1.0f, 0.60f, 0.90f);
        pointLights.push_back(pl);
    }

    int sphereVerticesCount = sizeof(sphere) / sizeof(sphere[0]) / 6;
    auto sphereModel = make_shared<Model>(sphere, sphereVerticesCount);
    for (size_t i = 0; i < pointLights.size(); ++i) {
        const auto &pl = pointLights[i];
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(pl.getPosition()));
    t->add(make_shared<Scale>(glm::vec3(0.06f)));
        auto lightObj = make_shared<DrawableObject>(sphereModel, shader, t);
        scene2.push_back(lightObj);
        lightMarkers.push_back(lightObj);
    }
    return scene2;
}


vector<shared_ptr<DrawableObject>> Scene::initializeScene3(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene3;

    auto light = make_shared<Light>(glm::vec3(0.0f), glm::vec3(1.0f), 1.2f);
    glm::vec3 viewPos(0.0f, 0.0f, 3.0f);

    glm::vec3 colors[4] = {
        {0.6f, 0.8f, 1.0f},
        {0.5f, 0.7f, 1.0f},
        {0.4f, 0.6f, 1.0f},
        {0.3f, 0.5f, 1.0f}
    };

    glm::vec3 positions[4] = {
        {0.0f,  0.6f, 0.0f},
        {0.0f, -0.6f, 0.0f},
        {-0.6f, 0.0f, 0.0f},
        {0.6f,  0.0f, 0.0f}
    };

    int sphereVerticesCount = sizeof(sphere) / sizeof(sphere[0]) / 6;

    for (int i = 0; i < 4; ++i) {
        auto trans = make_shared<Transformation>();
        trans->add(make_shared<Translate>(positions[i]));
        trans->add(make_shared<Scale>(glm::vec3(0.2f)));

        auto model = make_shared<Model>(sphere, sphereVerticesCount);
        auto obj = make_shared<DrawableObject>(model, shader, trans);

        shader->use();
        light->applyToShader(shader);
        shader->setUniform("viewPosition", viewPos);
        shader->setUniform("objectColor", colors[i]);

        scene3.push_back(obj);
    }
    return scene3;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene4(shared_ptr<ProgramShader> shader, shared_ptr<ProgramShader> sunShader) {
    vector<shared_ptr<DrawableObject>> scene4;

    int sphereVerticesCount = sizeof(sphere) / sizeof(sphere[0]) / 6;
    auto sphereModel = make_shared<Model>(sphere, sphereVerticesCount);

    auto sunTrans = make_shared<Transformation>();
    sunTrans->add(make_shared<Scale>(glm::vec3(0.5f)));
    auto sunObj = make_shared<DrawableObject>(sphereModel, sunShader, sunTrans);
    scene4.push_back(sunObj);

    auto earthTrans = make_shared<Transformation>();
    earthTrans->add(make_shared<Translate>(glm::vec3(3.0f, 0.0f, 0.0f)));
    earthTrans->add(make_shared<Scale>(glm::vec3(0.2f)));
    auto earthObj = make_shared<DrawableObject>(sphereModel, shader, earthTrans);
    earthObj->setColor(glm::vec3(0.2f, 0.4f, 1.0f));
    scene4.push_back(earthObj);

    auto moonTrans = make_shared<Transformation>();
    moonTrans->add(make_shared<Translate>(glm::vec3(3.6f, 0.0f, 0.0f)));
    moonTrans->add(make_shared<Scale>(glm::vec3(0.1f)));
    auto moonObj = make_shared<DrawableObject>(sphereModel, shader, moonTrans);
    moonObj->setColor(glm::vec3(0.7f, 0.7f, 0.7f));
    scene4.push_back(moonObj);

    solarSun = sunObj;
    solarEarth = earthObj;
    solarMoon = moonObj;

    return scene4;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene5(
    shared_ptr<ProgramShader> shaderConstant,
    shared_ptr<ProgramShader> shaderLambert,
    shared_ptr<ProgramShader> shaderPhong,
    shared_ptr<ProgramShader> shaderBlinn)
{
    vector<shared_ptr<DrawableObject>> scene5;

    int suziFlatCount   = sizeof(suziFlat) / sizeof(suziFlat[0]) / 6;
    int suziSmoothCount = sizeof(suziSmooth) / sizeof(suziSmooth[0]) / 6;

    auto suziFlatModel   = make_shared<Model>(suziFlat, suziFlatCount);
    auto suziSmoothModel = make_shared<Model>(suziSmooth, suziSmoothCount);

    glm::vec3 lightPos(0.0f, 2.0f, 3.0f);
    glm::vec3 lightColor(1.0f);
    glm::vec3 viewPos(0.0f, 0.0f, 5.0f);

    float startY = 1.5f;
    float deltaY = -1.0f;
    float deltaX = 1.5f;

    auto setupShader = [&](shared_ptr<ProgramShader> shader) {
        shader->use();
        shader->setUniform("lightPosition", lightPos);
        shader->setUniform("lightColor", lightColor);
        shader->setUniform("viewPosition", viewPos);
    };

    setupShader(shaderConstant);
    setupShader(shaderLambert);
    setupShader(shaderPhong);
    setupShader(shaderBlinn);

    auto addPair = [&](shared_ptr<ProgramShader> shader, float rowY) {
        auto tFlat = make_shared<Transformation>();
        tFlat->add(make_shared<Translate>(glm::vec3(-deltaX / 2.0f, rowY, 0.0f)));
        tFlat->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objFlat = make_shared<DrawableObject>(suziFlatModel, shader, tFlat);
        objFlat->setColor(glm::vec3(0.6f, 0.7f, 0.9f));
    
        scene5.push_back(objFlat);

        auto tSmooth = make_shared<Transformation>();
        tSmooth->add(make_shared<Translate>(glm::vec3(deltaX / 2.0f, rowY, 0.0f)));
        tSmooth->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objSmooth = make_shared<DrawableObject>(suziSmoothModel, shader, tSmooth);
        objSmooth->setColor(glm::vec3(0.6f, 0.7f, 0.9f));
        scene5.push_back(objSmooth);
    };

    addPair(shaderConstant, startY + 0 * deltaY);
    addPair(shaderLambert,  startY + 1 * deltaY);
    addPair(shaderPhong,    startY + 2 * deltaY);
    addPair(shaderBlinn,    startY + 3 * deltaY);

    auto phongNormShader = make_shared<ProgramShader>(
        "Shaders/phongNorm.vert",
        "Shaders/phongNorm.frag"
    );

    phongNormShader->use();
    phongNormShader->setUniform("lightPosition", glm::vec3(0.0f, 2.0f, 2.0f));
    phongNormShader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    phongNormShader->setUniform("objectColor", glm::vec3(0.6f, 0.7f, 0.9f));
    phongNormShader->setUniform("viewPosition", viewPos);

    auto suziNormModel = make_shared<Model>(suziSmooth, suziSmoothCount);

    auto tPhongNorm = make_shared<Transformation>();
    tPhongNorm->add(make_shared<Translate>(glm::vec3(2.2f, startY + 2 * deltaY, 0.0f)));
    tPhongNorm->add(make_shared<Scale>(glm::vec3(0.5f)));

    auto suziPhongNorm = make_shared<DrawableObject>(suziNormModel, phongNormShader, tPhongNorm);
    suziPhongNorm->setColor(glm::vec3(0.6f, 0.7f, 0.9f));
    scene5.push_back(suziPhongNorm);

    try {
        auto carModel = Model::loadOBJ("Models/formula1.obj");
        auto tCar = make_shared<Transformation>();
    tCar->add(make_shared<Translate>(glm::vec3(3.5f, startY + 2 * deltaY - 3.2f, 0.0f)));
        tCar->add(make_shared<Scale>(glm::vec3(0.1f)));
        auto carObj = make_shared<DrawableObject>(carModel, shaderPhong, tCar);
        carObj->setColor(glm::vec3(0.6f, 0.7f, 0.9f));
        scene5.push_back(carObj);
    } catch (const std::exception &e) {
        std::cerr << "Nepodařilo se načíst formula1.obj: " << e.what() << std::endl;
    }
    return scene5;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene6(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene;

    shader->use();

    shader->setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->setUniform("dirLight.color",     glm::vec3(1.0f, 1.0f, 1.0f));

    shader->setUniform("materialSpecular", glm::vec3(0.5f));
    shader->setUniform("materialShininess", 32.0f);

    const float planeVertices[] = {
         1,0, 1,    0,1,0,      1,1,
         1,0,-1,    0,1,0,      1,0,
        -1,0,-1,    0,1,0,      0,0,

        -1,0, 1,    0,1,0,      0,1,
         1,0, 1,    0,1,0,      1,1,
        -1,0,-1,    0,1,0,      0,0
    };

    auto planeModel = make_shared<Model>(planeVertices, 6, true);

    auto tPlane = make_shared<Transformation>();
    tPlane->add(make_shared<Scale>(glm::vec3(5.0f)));

    auto plane = make_shared<DrawableObject>(planeModel, shader, tPlane);
    scene.push_back(plane);

    planeTexture = make_shared<Texture>("Models/grass.png");
    plane->setTexture(planeTexture);
    shader->setUniform("diffuseTex", 0);

    try {
        auto model = Model::loadOBJ("Models/formula1.obj");

        auto tr = make_shared<Transformation>();
        tr->add(make_shared<Translate>(glm::vec3(0, 0.02f, 0)));
        tr->add(make_shared<Scale>(glm::vec3(0.1f)));

        auto f1 = make_shared<DrawableObject>(model, shader, tr);
        f1->setColor(glm::vec3(0.9f, 0.2f, 0.2f));
        scene.push_back(f1);

    } catch (...) {}

    try {
        auto cube = Model::loadOBJ("Models/cube.obj");

        auto tr = make_shared<Transformation>();
        tr->add(make_shared<Translate>(glm::vec3(1.5f, 0.5f, -3)));

        auto c = make_shared<DrawableObject>(cube, shader, tr);
        c->setColor(glm::vec3(0.5f, 0.6f, 0.85f));
        scene.push_back(c);

    } catch (...) {}

    return scene;
}


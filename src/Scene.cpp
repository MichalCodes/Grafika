#include "Scene.h"
#include "../Models/sphere.h"
#include "../Models/plain.h"
#include "../Models/suzi_smooth.h"
#include "../Models/suzi_flat.h"
#include "../Models/tree.h"
#include "../Models/bushes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <cmath>

using namespace std;


static glm::vec3 bezierCubic(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) { // implementace výpočtu bezierovy kubiky
    float mt = 1.0f - t; // koeficient
    float b0 = mt * mt * mt;
    float b1 = 3.0f * mt * mt * t; // bernsteinovy polynomy
    float b2 = 3.0f * mt * t * t;
    float b3 = t * t * t;
    return b0 * p0 + b1 * p1 + b2 * p2 + b3 * p3;
}

Scene::Scene() : activeSceneIndex(0) {}

void Scene::addScene(const vector<shared_ptr<DrawableObject>>& sceneObjects) {
    scenes.push_back(sceneObjects);
}

void Scene::toggleFlashlight() {
    flashlightEnabled = !flashlightEnabled;
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
        auto shader = obj->getShader();
        shader->use();

        if (activeSceneIndex == 0) shader->setUniform("w", 1.0f);
        else if (activeSceneIndex == 1) shader->setUniform("w", 300.0f);
        else shader->setUniform("w", 1.0f);

        if (activeSceneIndex == 2) shader->setUniform("isDirectional", true);
        else shader->setUniform("isDirectional", false);
        
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        
        if (activeSceneIndex == 4) shader->setUniform("objectColor", glm::vec3(0.6f, 0.8f, 0.6f));

        if (obj->getTexture() && obj->getShader() == sunShader) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getID());
        }
        
        if (activeSceneIndex == 2 && obj->getTexture()) {
            glActiveTexture(GL_TEXTURE0); 
            glBindTexture(GL_TEXTURE_2D, obj->getTexture()->getID()); 
        }

        obj->draw(mvp);
    }

    if (activeSceneIndex == 2 && skyCube) {
        skyCube->render(view, projection);
    }
}

void Scene::drawForPicking(const glm::mat4& projection, const glm::mat4& view) const {
    if (activeSceneIndex < 0 || activeSceneIndex >= (int)scenes.size()) return;
    const auto& activeScene = scenes[activeSceneIndex];
    
    for (const auto& obj : activeScene) {
        if (activeSceneIndex == 2) {
            bool isMoleCube = false;
            for (const auto& m : moleCubes) {
                if (m.obj == obj) {
                    isMoleCube = true;
                    if (m.currentY < -0.8f) { 
                        continue;
                    }
                    break;
                }
            }
            if (obj->getID() == 1) {
            } else if (!isMoleCube) {
                 continue; 
            }
        }
        glm::mat4 model = obj->getTransform()->getMat();
        glm::mat4 mvp = projection * view * model;
        glStencilFunc(GL_ALWAYS, obj->getID(), 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        obj->drawForPicking(mvp);
    }
}
void Scene::handleMouseClick(float mouseX, float mouseY, const glm::mat4& view, const glm::mat4& projection, int viewportWidth, int viewportHeight) {
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); 
    drawForPicking(projection, view);
    glFlush();
    glFinish();
    glStencilMask(0x00);
    unsigned int pickedID;
    glReadPixels((int)mouseX, viewportHeight - (int)mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &pickedID);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST); 
    setSelectedObject(pickedID);
}

void Scene::setSelectedObject(unsigned int objectID) {
    if (objectID == 0) {
        selectedObject = nullptr;
        cout << "background" << endl;
        return;
    }
    shared_ptr<DrawableObject> foundObject = nullptr;
    const auto& activeScene = scenes[activeSceneIndex];
    for (const auto& obj : activeScene) {
        if (obj->getID() == objectID) {
            foundObject = obj;
            break;
        }
    }

    if (foundObject) {
        if (foundObject == formulaInScene6) cout << "formula1" << endl;
        else if (foundObject == cubeInScene6) cout << "cube" << endl;
        else if (objectID == 1 && activeSceneIndex == 2) {
            cout << "Plain / Grass" << endl;
        } 
        else if (activeSceneIndex == 2) {
            if (objectID == formula.obj->getID()) {
                if (formula.isActive) {
                    currentScore += 2;
                    cout << "SCORE: " << currentScore << endl;
                    cout << "Formula HIT! (+2 Points)" << endl;
                    
                    formula.isActive = false; 
                    formula.obj->getTransform()->clear();
                    formula.obj->getTransform()->add(make_shared<Translate>(formula.startPos));
                    formula.obj->getTransform()->add(make_shared<Rotate>(glm::radians(-90.0f), glm::vec3(0, 1, 0)));
                    formula.obj->getTransform()->add(make_shared<Scale>(glm::vec3(0.1f)));
                }
            } else { 
                for (auto &m : moleCubes) {
                    if (m.obj->getID() == objectID) {
                        if (!m.isHit && m.emerging) {
                            currentScore++;
                            cout << "SCORE: " << currentScore << endl;
                        }
                        m.isHit = true;
                        m.obj->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
                        m.emerging = false;
                        m.stayTimer = 0.0f;
                        cout << "MoleCube HIT! ID: " << objectID << endl;
                        break;
                    }
                }
            }
        }
        else {
            cout << "Object ID: " << objectID << endl;
        }
        selectedObject = foundObject;
    } else {
        selectedObject = nullptr;
        cout << "Object ID: " << objectID << " (Not found in active scene objects list)" << endl;
    }
}

void Scene::update(float time, shared_ptr<ProgramShader> shader, shared_ptr<Camera> camera) {
    float deltaTime = time - lastUpdateTime;
    lastUpdateTime = time;
    if (deltaTime < 0.0f || deltaTime > 1.0f) deltaTime = 0.016f;
    
    if (activeSceneIndex == 6 && triangleObject) {
        auto composite = make_shared<Transformation>();
        composite->add(make_shared<Translate>(glm::vec3(0.0f, -0.2f, 0.0f)));
        composite->add(make_shared<Rotate>(time, glm::vec3(0.0f, 0.0f, 1.0f)));
        triangleObject->setTransformation(composite);
    }

    if (activeSceneIndex == 2) {
        if (moleCubes.empty()) return;

        moleTimer += deltaTime;
        static std::default_random_engine rng{std::random_device{}()};

        if (moleTimer > 2.0f) {
            moleTimer = 0.0f;
            std::uniform_int_distribution<int> dist(0, static_cast<int>(moleCubes.size()) - 1);
            int idx = dist(rng);
            if (!moleCubes[idx].emerging && moleCubes[idx].currentY <= -0.9f) {
                moleCubes[idx].emerging = true;
                moleCubes[idx].stayTimer = 1.2f;
            }
        }

        for (auto &m : moleCubes) {
            if (m.emerging) {
                m.currentY += m.speed * deltaTime;
                if (m.currentY >= m.baseY) {
                    m.currentY = m.baseY;
                    m.stayTimer -= deltaTime;
                    if (m.stayTimer <= 0.0f) {
                        m.emerging = false;
                    }
                }
            } else {
                if (m.currentY > -1.0f) {
                    m.currentY -= m.speed * deltaTime;
                    
                    if (m.currentY < -1.0f) {
                        m.currentY = -1.0f;
                        
                        if (m.isHit) {
                            m.isHit = false;
                            m.obj->setColor(glm::vec3(1.0f, 0.2f, 0.2f));
                        }
                    }
                }
            }

            auto t = make_shared<Transformation>();
            t->add(make_shared<Translate>(glm::vec3(m.baseX, m.currentY, m.baseZ)));
            t->add(make_shared<Scale>(glm::vec3(0.5f)));
            m.obj->setTransformation(t);
        }
        static float formulaSpawnTimer = 0.0f;
        formulaSpawnTimer += deltaTime;

        if (!formula.isActive && formulaSpawnTimer > 7.0f) {
            formula.isActive = true;
            formula.currentT = 0.0f; 
            formulaSpawnTimer = 0.0f; 
        }

        if (formula.isActive) {
            formula.currentT += formula.speed * deltaTime;
            float t = glm::clamp(formula.currentT, 0.0f, 1.0f);
            
            float planeY = formula.startPos.y;
            const float plainStart = formula.startPos.x;
            const float plainEnd = formula.endPos.x;
            const float plainWidth = plainEnd - plainStart;
            glm::vec3 p0(plainStart, planeY, formula.startPos.z); // ridici body
            glm::vec3 p1(plainStart + plainWidth * 0.33f, planeY, formula.startPos.z + 3.0f);
            glm::vec3 p2(plainStart + plainWidth * 0.66f, planeY, formula.startPos.z - 3.0f);
            glm::vec3 p3(plainEnd, planeY, formula.startPos.z);
            
            glm::vec3 currentPos = bezierCubic(p0, p1, p2, p3, t); // Bezier calculation
            
            auto t_trans = make_shared<Transformation>();
            t_trans->add(make_shared<Translate>(currentPos));
            t_trans->add(make_shared<Rotate>(glm::radians(180.0f), glm::vec3(0, 1, 0))); 
            t_trans->add(make_shared<Scale>(glm::vec3(0.1f)));
            formula.obj->setTransformation(t_trans);

            if (formula.currentT >= 1.0f) {
                formula.isActive = false;
            }
        }
    } else if (activeSceneIndex == 3) {
        if (!solarEarth || !solarMoon || !solarMars || !solarJupiter ||!solarSaturn || !solarUranus || !solarNeptune || !solarSun) return;

        {
            auto tSun = make_shared<Transformation>();
            tSun->add(make_shared<Rotate>(glm::radians(time * 5.0f), glm::vec3(0, 1, 0)));
            tSun->add(make_shared<Scale>(glm::vec3(1.6f)));
            solarSun->setTransformation(tSun);
        }

        struct PlanetOrbit { // tady menit orbity planet
            shared_ptr<DrawableObject> planet;
            float radius;
            float speed;
            float size;
            float spin;
        };

        vector<PlanetOrbit> orbits = {
            {solarEarth,   3.0f, 1.0f, 0.20f,  40.0f},
            {solarMoon,    0.6f, 3.0f, 0.10f,  10.0f},
            {solarMars,    4.2f, 0.8f, 0.15f,  35.0f},
            {solarJupiter, 5.5f, 0.5f, 0.40f,  25.0f},
            {solarSaturn,  7.0f, 0.4f, 0.35f,  20.0f},
            {solarUranus,  8.5f, 0.3f, 0.25f,  15.0f},
            {solarNeptune,10.0f, 0.2f, 0.25f,  12.0f}
        };

        glm::vec3 earthPos(
            orbits[0].radius * cos(time * orbits[0].speed),
            orbits[0].radius * sin(time * orbits[0].speed),
            0.0f
        );

        auto earthTrans = make_shared<Transformation>();
        earthTrans->add(make_shared<Translate>(earthPos));
        earthTrans->add(make_shared<Rotate>(glm::radians(time * orbits[0].spin), glm::vec3(0,1,0)));
        earthTrans->add(make_shared<Scale>(glm::vec3(orbits[0].size)));
        solarEarth->setTransformation(earthTrans);

        glm::vec3 moonPos(earthPos.x + orbits[1].radius * cos(time * orbits[1].speed),earthPos.y + orbits[1].radius * sin(time * orbits[1].speed),0.0f);

        auto moonTrans = make_shared<Transformation>();
        moonTrans->add(make_shared<Translate>(moonPos));
        moonTrans->add(make_shared<Rotate>(glm::radians(time * orbits[1].spin), glm::vec3(0,1,0)));
        moonTrans->add(make_shared<Scale>(glm::vec3(orbits[1].size)));
        solarMoon->setTransformation(moonTrans);

        for (size_t i = 2; i < orbits.size(); ++i) {
            auto& o = orbits[i];
            glm::vec3 pos( o.radius * cos(time * o.speed), o.radius * sin(time * o.speed), 0.0f);
            auto trans = make_shared<Transformation>();
            trans->add(make_shared<Translate>(pos));
            trans->add(make_shared<Rotate>(glm::radians(time * o.spin), glm::vec3(0,1,0)));
            trans->add(make_shared<Scale>(glm::vec3(o.size)));
            o.planet->setTransformation(trans);
        }
    } else if (activeSceneIndex == 4) {
        if (!forestShader) return; 
        forestShader->use();

        glm::vec3 camPos = camera->getPosition();
        forestShader->setUniform("viewPos", camPos);

        forestShader->setUniform("flashlight.position", camPos);
        forestShader->setUniform("flashlight.direction", camera->getFront());
        if (flashlightEnabled) {
            forestShader->setUniform("flashlight.color", glm::vec3(1.0f, 1.0f, 0.95f));
        } else {
            forestShader->setUniform("flashlight.color", glm::vec3(0.0f));
        }
        forestShader->setUniform("flashlight.cutoff", glm::cos(glm::radians(12.5f)));
        forestShader->setUniform("flashlight.outerCutoff", glm::cos(glm::radians(17.5f)));

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
            forestShader->setUniform("numLights", static_cast<int>(pointLights.size()));
            for (size_t i = 0; i < pointLights.size(); ++i) {
                const auto &pl = pointLights[i];
                std::string idx = "pointLights[" + std::to_string(i) + "]";
                forestShader->setUniform(idx + ".position", pl.getPosition());
                forestShader->setUniform(idx + ".color", pl.getColor() * pl.getIntensity());
                forestShader->setUniform(idx + ".constant", pl.getConstant());
                forestShader->setUniform(idx + ".linear", pl.getLinear());
                forestShader->setUniform(idx + ".quadratic", pl.getQuadratic()); // nastavuju utlumení
            }
        } else {
            if (fireflies.empty()) {
                fireflies.push_back({ glm::vec3(-2.0f, 0.4f, -2.0f), glm::vec3(1.0f, 0.7f, 0.5f) });
                fireflies.push_back({ glm::vec3( 2.0f, 0.6f, -1.0f), glm::vec3(0.5f, 0.7f, 1.0f) });
                fireflies.push_back({ glm::vec3(-1.5f, 0.5f,  2.0f), glm::vec3(0.8f, 1.0f, 0.6f) });
                fireflies.push_back({ glm::vec3( 1.2f, 0.5f,  1.8f), glm::vec3(1.0f, 0.6f, 0.8f) });
            }
            forestShader->setUniform("numLights", static_cast<int>(fireflies.size()));
            for (size_t i = 0; i < fireflies.size(); ++i) {
                const auto& f = fireflies[i];
                std::string idx = "pointLights[" + std::to_string(i) + "]";
                forestShader->setUniform(idx + ".position", f.position);
                forestShader->setUniform(idx + ".color", f.color);
                forestShader->setUniform(idx + ".constant", 1.0f);
                forestShader->setUniform(idx + ".linear", 0.045f);
                forestShader->setUniform(idx + ".quadratic", 0.0075f);
            }
        }
    }

    if (activeSceneIndex == 0 || activeSceneIndex == 1) {
        if (universalShader) {
            universalShader->use();
            universalShader->setUniform("lightPosition", sceneLightPos);
        }
    } else if (activeSceneIndex == 2) {
        if (universalShader) {
            universalShader->use();
            universalShader->setUniform("lightPosition", scene3DirectionalPos);
        }
    }
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene1(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene1;
    this->universalShader = shader;
    sceneLightPos = glm::vec3(0.0f, 0.3f, 2.0f);
    shader->use();
    shader->setUniform("lightPosition", sceneLightPos);
    
    auto t1 = make_shared<Transformation>();
    t1->add(make_shared<Translate>(glm::vec3(1.0f, 0.0f, -2.0f)));

    auto t2 = make_shared<Transformation>();
    t2->add(make_shared<Translate>(glm::vec3(1.0f, 0.0f, -2.0f)));

    glm::mat4 customMat(1.0f);
    customMat[3][3] = 20.0f;
    t2->add(make_shared<CustomTransform>(customMat));

    auto suziModel = make_shared<Model>(suziSmooth, sizeof(suziSmooth)/sizeof(float)/6);

    auto obj1 = make_shared<DrawableObject>(suziModel, shader, t1);
    obj1->setColor(glm::vec3(0.8f, 0.5f, 0.2f));
    scene1.push_back(obj1);

    auto obj2 = make_shared<DrawableObject>(suziModel, shader, t2);
    obj2->setColor(glm::vec3(0.2f, 0.5f, 0.8f));
    scene1.push_back(obj2);

    return scene1;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene2(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene2;
    this->universalShader = shader;
    sceneLightPos = glm::vec3(-10.0f, 2.0f, 10.0f);
    shader->use();
    shader->setUniform("lightPosition", sceneLightPos);

    auto t1 = make_shared<Transformation>();
    t1->add(make_shared<Translate>(glm::vec3(1.0f, 0.0f, -2.0f)));

    auto t2 = make_shared<Transformation>();
    t2->add(make_shared<Translate>(glm::vec3(1.0f, 0.0f, -2.0f)));

    glm::mat4 customMat(1.0f);
    customMat[3][3] = 20.0f;
    t2->add(make_shared<CustomTransform>(customMat));

    auto suziModel = make_shared<Model>(suziSmooth, sizeof(suziSmooth)/sizeof(float)/6);

    auto obj1 = make_shared<DrawableObject>(suziModel, shader, t1);
    obj1->setColor(glm::vec3(0.8f, 0.5f, 0.2f));
    scene2.push_back(obj1);

    auto obj2 = make_shared<DrawableObject>(suziModel, shader, t2);
    obj2->setColor(glm::vec3(0.2f, 0.5f, 0.8f));
    scene2.push_back(obj2);

    return scene2;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene3(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene3;
    currentScore = 0;
    this->universalShader = shader;
    scene3DirectionalPos = glm::vec3(0.0f, 10.0f, 0.0f);
    shader->use();
    shader->setUniform("lightPosition", scene3DirectionalPos);

    const int vertexCount = sizeof(plane) / sizeof(float) / 6;
    vector<float> data;
    data.reserve(vertexCount * 8);

    const float tile = 6.0f;
    for (int i = 0; i < vertexCount; ++i) {
        float x = plane[i*6 + 0];
        float y = plane[i*6 + 1];
        float z = plane[i*6 + 2];
        float nx = plane[i*6 + 3];
        float ny = plane[i*6 + 4];
        float nz = plane[i*6 + 5];

        float u = (x + 5.0f) / 10.0f * tile;
        float v = (z + 5.0f) / 10.0f * tile;

        data.insert(data.end(), {x, y, z, nx, ny, nz, u, v});
    }

    auto planeModel = make_shared<Model>(data.data(), vertexCount, true);
    auto tplane = make_shared<Transformation>();
    tplane->add(make_shared<Scale>(glm::vec3(10.0f, 1.0f, 10.0f)));

    auto planeObj = make_shared<DrawableObject>(planeModel, shader, tplane);
    planeObj->setTexture(make_shared<Texture>("Textures/grass.png"));
    planeObj->setID(1);
    scene3.push_back(planeObj);

    auto cubeModel = Model::loadOBJ("Models/cube.obj");

    vector<glm::vec3> positions = {
        {-3, 0, -3}, {0, 0, -3}, {3, 0, -3},
        {-3, 0,  0}, {0, 0,  0}, {3, 0,  0},
        {-3, 0,  3}, {0, 0,  3}, {3, 0,  3}
    };

    moleCubes.clear();
    unsigned int cubeID = 2;
    for (auto& pos : positions) {
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(glm::vec3(pos.x, -1.0f, pos.z)));
        t->add(make_shared<Scale>(glm::vec3(0.5f)));

        auto cube = make_shared<DrawableObject>(cubeModel, shader, t);
        cube->setColor(glm::vec3(1.0f, 0.2f, 0.2f));
        cube->setID(cubeID++);

        scene3.push_back(cube);

        MoleCube m;
        m.obj = cube;
        m.baseX = pos.x;
        m.baseZ = pos.z;
        m.baseY = 0.5f;
        m.currentY = -1.0f;
        m.emerging = false;
        m.speed = 2.5f;
        m.stayTimer = 0.0f;
        m.isHit = false;
        moleCubes.push_back(m);
    }
    auto formulaModel = Model::loadOBJ("Models/formula1.obj"); 
    
    const float trackZ = 5.0f;
    
    glm::vec3 formulaStartPos(-8.0f, 0.2f, trackZ); 

    auto tFormula = make_shared<Transformation>();
    tFormula->add(make_shared<Translate>(formulaStartPos));
    tFormula->add(make_shared<Rotate>(glm::radians(90.0f), glm::vec3(0, 1, 0))); 
    tFormula->add(make_shared<Scale>(glm::vec3(0.1f)));
    
    auto formulaObj = make_shared<DrawableObject>(formulaModel, shader, tFormula);
    formulaObj->setColor(glm::vec3(1.0f, 0.0f, 0.0f)); 
    formulaObj->setID(11); 

    scene3.push_back(formulaObj);

    formula.obj = formulaObj;
    formula.startPos = formulaStartPos;
    formula.endPos = glm::vec3(8.0f, 0.2f, trackZ);
    formula.currentT = 0.0f;
    formula.isActive = false; 
    return scene3;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene4(shared_ptr<ProgramShader> shader, shared_ptr<ProgramShader> sunShader){
    vector<shared_ptr<DrawableObject>> scene4;
    this->sunShader = sunShader;

    shader->use();
    shader->setUniform("light.position", glm::vec3(0.0f, 0.0f, 0.0f));
    shader->setUniform("light.color",    glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setUniform("viewPos",        glm::vec3(0.0f, 3.0f, 10.0f));

    shader->setUniform("light.constant",  1.0f);
    shader->setUniform("light.linear",    0.09f);
    shader->setUniform("light.quadratic", 0.032f);

    sunShader->use();
    sunShader->setUniform("sunTexture", 0);

    auto makeTexturedSphere = [&]() -> shared_ptr<Model>{
        const int floatsPerVertex = 6;
        int vertexCount = 17280 / floatsPerVertex;

        vector<float> u_coords(vertexCount);
        vector<float> v_coords(vertexCount);
        
        for (int i = 0; i < vertexCount; ++i){
            float x  = sphere[i*6 + 0];
            float y  = sphere[i*6 + 1];
            float z  = sphere[i*6 + 2];

            float v = acos(y) / M_PI; // vertikální UV
            float u = 0.5f + atan2(z, x) / (2.0f * M_PI); // horizontalni UV
            
            u_coords[i] = u;
            v_coords[i] = v;
        }
        
        for (int i = 0; i < vertexCount; i += 3) {
            float u1 = u_coords[i];
            float u2 = u_coords[i+1];
            float u3 = u_coords[i+2];

            float u_min = std::min(u1, std::min(u2, u3));
            float u_max = std::max({u1, u2, u3});
            
            if (u_max - u_min > 0.5f) {
                for (int j = 0; j < 3; ++j) {
                    int index = i + j;
                    float current_u = u_coords[index];
                    
                    if (current_u < 0.5f) { 
                        u_coords[index] += 1.0f;
                    }
                }
            }
        }
        
        vector<float> data;
        data.reserve(static_cast<size_t>(vertexCount) * 8);

        for (int i = 0; i < vertexCount; ++i) {
            float x  = sphere[i*6 + 0];
            float y  = sphere[i*6 + 1];
            float z  = sphere[i*6 + 2];
            float nx = sphere[i*6 + 3];
            float ny = sphere[i*6 + 4];
            float nz = sphere[i*6 + 5];

            data.push_back(x);
            data.push_back(y);
            data.push_back(z);
            data.push_back(nx);
            data.push_back(ny);
            data.push_back(nz);
            data.push_back(u_coords[i]);
            data.push_back(v_coords[i]);
        }
        return make_shared<Model>(data.data(), vertexCount, true);
    };

    auto sphereModel = makeTexturedSphere();
    auto loginModel = Model::loadOBJ("Models/login.obj");

    auto makePlanet = [&](const string& name, float distance, float scale, glm::vec3 color, bool isLogin = false){
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(glm::vec3(distance, 0.0f, 0.0f)));
        t->add(make_shared<Scale>(glm::vec3(scale)));

        shared_ptr<DrawableObject> obj;
        if (isLogin) {
            obj = make_shared<DrawableObject>(loginModel, shader, t);
        } else {
            obj = make_shared<DrawableObject>(sphereModel, shader, t);
        }

        string texPath = "Textures/" + name + ".jpg";
        auto tex = make_shared<Texture>(texPath);
        obj->setTexture(tex);
        obj->setColor(color);

        return obj;
    };

    {
        auto tSun = make_shared<Transformation>();
        tSun->add(make_shared<Scale>(glm::vec3(1.0f)));

        auto sunObj = make_shared<DrawableObject>(sphereModel, sunShader, tSun);
        auto sunTex = make_shared<Texture>("Textures/sun.jpg");
        sunObj->setTexture(sunTex);

        sunShader->use();
        sunShader->setUniform("hasTexture", 1);

        sunObj->setColor(glm::vec3(1.0f, 0.8f, 0.2f));
        scene4.push_back(sunObj);
        solarSun = sunObj;
    }
    shader->use(); 

    auto earth   = makePlanet("earth",   4.0f, 0.35f, glm::vec3(0.2f, 0.3f, 1.0f));
    auto mars    = makePlanet("mars",    5.0f, 0.55f, glm::vec3(1.0f, 0.3f, 0.1f), true);
    auto jupiter = makePlanet("jupiter", 7.0f, 1.20f, glm::vec3(1.0f, 0.8f, 0.5f));
    auto saturn  = makePlanet("saturn", 10.0f, 0.95f, glm::vec3(0.9f, 0.8f, 0.6f));
    auto uranus  = makePlanet("uranus", 13.0f, 1.00f, glm::vec3(0.6f, 0.8f, 1.0f));
    auto neptune = makePlanet("neptune",16.0f, 1.00f, glm::vec3(0.4f, 0.5f, 1.0f));
    auto moon    = makePlanet("moon",    4.0f + 0.6f, 0.10f, glm::vec3(0.8f, 0.8f, 0.8f));
    
    scene4.push_back(earth);
    scene4.push_back(moon);
    scene4.push_back(mars);
    scene4.push_back(jupiter);
    scene4.push_back(saturn);
    scene4.push_back(uranus);
    scene4.push_back(neptune);
    
    solarEarth   = earth;
    solarMoon    = moon;
    solarMars    = mars;
    solarJupiter = jupiter;
    solarSaturn  = saturn;
    solarUranus  = uranus;
    solarNeptune = neptune;
    
    return scene4;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene5(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene2;
    this->forestShader = shader;
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
        pl.setAttenuation(1.0f, 0.60f, 0.90f);           // utlum světlušek constant, linear quadratic
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

vector<shared_ptr<DrawableObject>> Scene::initializeScene6(
    shared_ptr<ProgramShader> pbr_like_shader,
    shared_ptr<ProgramShader> lambertShader)
{
    vector<shared_ptr<DrawableObject>> scene6;

    int suziFlatCount   = sizeof(suziFlat) / sizeof(suziFlat[0]) / 6;
    int suziSmoothCount = sizeof(suziSmooth) / sizeof(suziSmooth[0]) / 6;
    auto suziFlatModel   = make_shared<Model>(suziFlat, suziFlatCount);
    auto suziSmoothModel = make_shared<Model>(suziSmooth, suziSmoothCount);
    int sphereVerticesCount = sizeof(sphere) / sizeof(sphere[0]) / 6;
    auto sphereModel     = make_shared<Model>(sphere, sphereVerticesCount);

    const glm::vec3 UNIFORM_COLOR(0.6f, 0.7f, 0.9f);

    glm::vec3 viewPos(0.0f, 0.0f, 5.0f);
    float suziZ = 4.0f; 
    float startY = 1.5f; 
    float deltaY = -1.0f;
    float deltaX = 1.5f;
    const float SUZI_ROTATION = glm::radians(180.0f); 

    if (this->sunShader) { 
        this->sunShader->use();
        this->sunShader->setUniform("viewPosition", viewPos);
        this->sunShader->setUniform("hasTexture", 0); 
    }

    if (pbr_like_shader) {
        pbr_like_shader->use();
        pbr_like_shader->setUniform("lightColor", glm::vec3(1.0f));
        pbr_like_shader->setUniform("viewPosition", viewPos);
    }

    glm::vec3 spherePositions[4] = {
        {0.0f,  0.6f, -4.0f}, {0.0f, -0.6f, -4.0f},
        {-0.6f, 0.0f, -4.0f}, {0.6f,  0.0f, -4.0f}
    };

    glm::vec3 lightPosSpheres(0.0f, 0.0f, -4.0f);

    if (pbr_like_shader) {
        pbr_like_shader->use();
        pbr_like_shader->setUniform("lightPosition", lightPosSpheres);
        pbr_like_shader->setUniform("lightingModel", 2);
        pbr_like_shader->setUniform("materialShininess", 32.0f); 
    }
    
    for (int i = 0; i < 4; ++i) {
        auto trans = make_shared<Transformation>();
        trans->add(make_shared<Translate>(spherePositions[i]));
        trans->add(make_shared<Scale>(glm::vec3(0.2f)));

        auto obj = make_shared<DrawableObject>(sphereModel, pbr_like_shader, trans);
        obj->setColor(UNIFORM_COLOR);

        scene6.push_back(obj);
    }
    
    glm::vec3 lightPosMonkeys = lightPosSpheres;

    if (this->sunShader) {
        this->sunShader->use();
        this->sunShader->setUniform("hasTexture", 0);
    }

    {
        auto tFlat = make_shared<Transformation>();
        tFlat->add(make_shared<Translate>(glm::vec3(-deltaX / 2.0f, startY + 0 * deltaY, suziZ)));
        tFlat->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tFlat->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objFlat = make_shared<DrawableObject>(suziFlatModel, this->sunShader, tFlat);
        objFlat->setColor(UNIFORM_COLOR);
        scene6.push_back(objFlat);
    }

    {
        auto tSmooth = make_shared<Transformation>();
        tSmooth->add(make_shared<Translate>(glm::vec3(deltaX / 2.0f, startY + 0 * deltaY, suziZ)));
        tSmooth->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tSmooth->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objSmooth = make_shared<DrawableObject>(suziSmoothModel, this->sunShader, tSmooth);
        objSmooth->setColor(UNIFORM_COLOR);
        scene6.push_back(objSmooth);
    }

    if (lambertShader) {
        lambertShader->use();
        lambertShader->setUniform("lightPosition", lightPosMonkeys);
        lambertShader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    }

    {
        auto tFlat = make_shared<Transformation>();
        tFlat->add(make_shared<Translate>(glm::vec3(-deltaX / 2.0f, startY + 1 * deltaY, suziZ)));
        tFlat->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tFlat->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objFlat = make_shared<DrawableObject>(suziFlatModel, lambertShader, tFlat);
        objFlat->setColor(UNIFORM_COLOR);
        scene6.push_back(objFlat);
    }

    {
        auto tSmooth = make_shared<Transformation>();
        tSmooth->add(make_shared<Translate>(glm::vec3(deltaX / 2.0f, startY + 1 * deltaY, suziZ)));
        tSmooth->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tSmooth->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objSmooth = make_shared<DrawableObject>(suziSmoothModel, lambertShader, tSmooth);
        objSmooth->setColor(UNIFORM_COLOR);
        scene6.push_back(objSmooth);
    }

    if (pbr_like_shader) {
        pbr_like_shader->use();
        pbr_like_shader->setUniform("lightPosition", lightPosMonkeys);
        pbr_like_shader->setUniform("lightingModel", 1);
        pbr_like_shader->setUniform("materialShininess", 64.0f);
        pbr_like_shader->setUniform("materialSpecularStrength", 0.5f);
        pbr_like_shader->setUniform("useNormalMatrix", 0);
    }

    {
        auto tFlat = make_shared<Transformation>();
        tFlat->add(make_shared<Translate>(glm::vec3(-deltaX / 2.0f, startY + 2 * deltaY, suziZ)));
        tFlat->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tFlat->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objFlat = make_shared<DrawableObject>(suziFlatModel, pbr_like_shader, tFlat);
        objFlat->setColor(UNIFORM_COLOR);
        scene6.push_back(objFlat);
    }

    {
        auto tSmooth = make_shared<Transformation>();
        tSmooth->add(make_shared<Translate>(glm::vec3(deltaX / 2.0f, startY + 2 * deltaY, suziZ)));
        tSmooth->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tSmooth->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objSmooth = make_shared<DrawableObject>(suziSmoothModel, pbr_like_shader, tSmooth);
        objSmooth->setColor(UNIFORM_COLOR);
        scene6.push_back(objSmooth);
    }


    if (pbr_like_shader) {
        pbr_like_shader->use();
        pbr_like_shader->setUniform("lightPosition", lightPosMonkeys);
        pbr_like_shader->setUniform("lightingModel", 2);
        pbr_like_shader->setUniform("materialShininess", 128.0f);
        pbr_like_shader->setUniform("materialSpecularStrength", 0.5f);
        pbr_like_shader->setUniform("useNormalMatrix", 0);
    }

    {
        auto tFlat = make_shared<Transformation>();
        tFlat->add(make_shared<Translate>(glm::vec3(-deltaX / 2.0f, startY + 3 * deltaY, suziZ)));
        tFlat->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tFlat->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objFlat = make_shared<DrawableObject>(suziFlatModel, pbr_like_shader, tFlat);
        objFlat->setColor(UNIFORM_COLOR);
        scene6.push_back(objFlat);
    }

    {
        auto tSmooth = make_shared<Transformation>();
        tSmooth->add(make_shared<Translate>(glm::vec3(deltaX / 2.0f, startY + 3 * deltaY, suziZ)));
        tSmooth->add(make_shared<Rotate>(SUZI_ROTATION, glm::vec3(0, 1, 0)));
        tSmooth->add(make_shared<Scale>(glm::vec3(0.5f)));
        auto objSmooth = make_shared<DrawableObject>(suziSmoothModel, pbr_like_shader, tSmooth);
        objSmooth->setColor(UNIFORM_COLOR);
        scene6.push_back(objSmooth);
    }
    return scene6;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene7(shared_ptr<ProgramShader> shader){
    float triangleVertices[] = {
         0.0f,  0.4f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.433f, -0.25f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.433f, -0.25f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    auto triangleModel = make_shared<Model>(triangleVertices, 3);
    triangleObject = make_shared<DrawableObject>(triangleModel, shader, make_shared<Transformation>());
    return {triangleObject};
}

void Scene::initializeSkyCube(shared_ptr<ProgramShader> skyboxShader) {
    skyCube = make_shared<SkyCube>();
    
    std::string texturePath = "Textures/";
    if (!skyCube->loadCubemap(
        texturePath + "posx.jpg",    // +X
        texturePath + "negx.jpg",    // -X
        texturePath + "negy.jpg",    // -Y 
        texturePath + "posy.jpg",    // +Y 
        texturePath + "posz.jpg",    // +Z
        texturePath + "negz.jpg"     // -Z
    )) {
        std::cerr << "Failed to load sky cube textures!" << std::endl;
        skyCube = nullptr;
        return;
    }
    
    skyCube->setShader(skyboxShader);
}
#include "Scene.h"
#include "../Models/sphere.h"
#include "../Models/plain.h"
#include "../Models/suzi_smooth.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <cmath>

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
        auto shader = obj->getShader();
        shader->use();
        if (activeSceneIndex == 0) shader->setUniform("w", 1.0f);
        else if (activeSceneIndex == 1) shader->setUniform("w", 300.0f);
        else shader->setUniform("w", 1.0f);
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        obj->draw(mvp);
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
            formula.currentX = formula.startPos.x; 
            formulaSpawnTimer = 0.0f; 
        }

        if (formula.isActive) {
            formula.currentX += formula.speed * deltaTime;
            const float visibleHeight = 0.5f;
            const float submergedDepth = -1.0f;
            const float plainStart = formula.startPos.x;
            const float plainEnd = formula.endPos.x;
            const float plainCenter = (plainStart + plainEnd) / 2.0f;
            
            float yPos;

            if (formula.currentX < plainCenter) {
                float progress = (formula.currentX - plainStart) / (plainCenter - plainStart);
                yPos = submergedDepth + progress * (visibleHeight - submergedDepth);
            } else {
                float progress = (formula.currentX - plainCenter) / (plainEnd - plainCenter);
                yPos = visibleHeight - progress * (visibleHeight - submergedDepth);
            }
            yPos = glm::max(submergedDepth, yPos); 
            glm::vec3 currentPos(formula.currentX, yPos, formula.startPos.z);
            auto t = make_shared<Transformation>();
            t->add(make_shared<Translate>(currentPos));
            t->add(make_shared<Rotate>(glm::radians(180.0f), glm::vec3(0, 1, 0))); 
            t->add(make_shared<Scale>(glm::vec3(0.1f)));
            formula.obj->setTransformation(t);

            if (formula.currentX >= formula.endPos.x) {
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

        struct PlanetOrbit {
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
        earthTrans->add(make_shared<Rotate>(glm::radians(time * orbits[0].spin), glm::vec3(0,1,0))); // rotace Země
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
    }
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene1(shared_ptr<ProgramShader> shader) {
    vector<shared_ptr<DrawableObject>> scene1;

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
    shader->use();
    shader->setUniform("lightPosition", glm::vec3(-3.0f, 6.0f, 3.0f));

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
    
    glm::vec3 formulaStartPos(-8.0f, -1.0f, trackZ); 

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
    formula.endPos = glm::vec3(8.0f, -1.0f, trackZ);
    formula.currentX = formulaStartPos.x;
    formula.isActive = false; 
    return scene3;
}

vector<shared_ptr<DrawableObject>> Scene::initializeScene4(shared_ptr<ProgramShader> shader, shared_ptr<ProgramShader> sunShader){
    vector<shared_ptr<DrawableObject>> scene4;
    shader->use();
    shader->setUniform("light.position", glm::vec3(0.0f, 0.0f, 0.0f));
    shader->setUniform("light.color",    glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setUniform("viewPos",        glm::vec3(0.0f, 3.0f, 10.0f));

    shader->setUniform("light.constant",  1.0f);
    shader->setUniform("light.linear",    0.09f);
    shader->setUniform("light.quadratic", 0.032f);

    auto makeTexturedSphere = [&]() -> shared_ptr<Model>{

        const int floatsPerVertex = 6;
        int vertexCount = 17280 / floatsPerVertex;

        vector<float> u_coords(vertexCount);
        vector<float> v_coords(vertexCount);
        
        for (int i = 0; i < vertexCount; ++i){
            float x  = sphere[i*6 + 0];
            float y  = sphere[i*6 + 1];
            float z  = sphere[i*6 + 2];

            float v = acos(y) / M_PI;
            float u = 0.5f + atan2(z, x) / (2.0f * M_PI);
            
            u_coords[i] = u;
            v_coords[i] = v;
        }
        
        for (int i = 0; i < vertexCount; i += 3) {
            float u1 = u_coords[i];
            float u2 = u_coords[i+1];
            float u3 = u_coords[i+2];

            float u_min = std::min(u1, std::min(u2, u3));
            float u_max = std::max(u1, std::max(u2, u3));
            
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

    auto makePlanet = [&](const string& name, float distance, float scale, glm::vec3 color){
        auto t = make_shared<Transformation>();
        t->add(make_shared<Translate>(glm::vec3(distance, 0.0f, 0.0f)));
        t->add(make_shared<Scale>(glm::vec3(scale)));

        auto obj = make_shared<DrawableObject>(sphereModel, shader, t);
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

        sunObj->setColor(glm::vec3(1.0f, 0.8f, 0.2f));
        scene4.push_back(sunObj);
        solarSun = sunObj;
    }

    auto earth   = makePlanet("earth",   4.0f, 0.35f, glm::vec3(0.2f, 0.3f, 1.0f));
    auto moon    = makePlanet("moon",    4.6f, 0.10f, glm::vec3(0.8f, 0.8f, 0.8f)); 
    auto mars    = makePlanet("mars",    5.0f, 0.25f, glm::vec3(1.0f, 0.3f, 0.1f));
    auto jupiter = makePlanet("jupiter", 7.0f, 1.20f, glm::vec3(1.0f, 0.8f, 0.5f));
    auto saturn  = makePlanet("saturn", 10.0f, 0.95f, glm::vec3(0.9f, 0.8f, 0.6f));
    auto uranus  = makePlanet("uranus", 13.0f, 1.00f, glm::vec3(0.6f, 0.8f, 1.0f));
    auto neptune = makePlanet("neptune",16.0f, 1.00f, glm::vec3(0.4f, 0.5f, 1.0f));

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
} // Scene.cpp

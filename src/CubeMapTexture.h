// #pragma once
// #include <memory>
// #include "Model.h"
// #include "ProgramShader.h"
// #include "Transformation.h"
// #include "Texture.h"
// #include <glm/glm.hpp> // Důležité pro glm::vec3

// using namespace std;

// class DrawableObject {
// public:
//     DrawableObject(shared_ptr<Model> m, 
//                    shared_ptr<ProgramShader> s, 
//                    shared_ptr<Transformation> t = nullptr)
//         : model(m), shader(s), transform(t ? t : make_shared<Transformation>()),
//           texture(nullptr) {}

//     void draw(const glm::mat4& mvp) const;

//     shared_ptr<Transformation> getTransform() const { return transform; }
//     void setTransformation(shared_ptr<Transformation> t) { transform = t; }

//     shared_ptr<ProgramShader> getShader() { return shader; }
//     shared_ptr<Model> getModel() const { return model; }

//     void setColor(const glm::vec3& color);
    
//     void setTexture(shared_ptr<Texture> tex) { texture = tex; }

//     glm::vec3 color = glm::vec3(1.0f); 

// private:
//     shared_ptr<Model> model;
//     shared_ptr<ProgramShader> shader;
//     shared_ptr<Transformation> transform;
//     shared_ptr<Texture> texture;
// };
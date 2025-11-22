#include "Model.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../tinyobjloader/tiny_obj_loader.h"

using namespace std;

Model::Model(const float* vertices, int count)
    : Model(vertices, count, false)
{}

Model::Model(const float* vertices, int count, bool hasTex)
    : hasTexcoords(hasTex), vertexCount(count)
{
    int stride = hasTexcoords ? 8 : 6;
    int strideBytes = stride * sizeof(float);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * strideBytes, vertices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)0);
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV 
    if (hasTexcoords) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideBytes, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Model::~Model() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void Model::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

shared_ptr<Model> Model::loadOBJ(const string& path)
{
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string warn, err;

    bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    if (!warn.empty()) cerr << "OBJ warning: " << warn << endl;
    if (!err.empty())  cerr << "OBJ error: " << err << endl;
    if (!ok) {
        cerr << "Chyba: nepodarilo se nacist OBJ: " << path << endl;
        return nullptr;
    }

    vector<float> data;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            
            data.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            data.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            data.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (!attrib.normals.empty()) {
                data.push_back(attrib.normals[3 * index.normal_index + 0]);
                data.push_back(attrib.normals[3 * index.normal_index + 1]);
                data.push_back(attrib.normals[3 * index.normal_index + 2]);
            } else {
                data.push_back(0);
                data.push_back(1);
                data.push_back(0);
            }

            if (!attrib.texcoords.empty()) {
                data.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                data.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            } else {
                data.push_back(0);
                data.push_back(0);
            }
        }
    }

    return make_shared<Model>(data.data(), data.size() / 8, true);
}

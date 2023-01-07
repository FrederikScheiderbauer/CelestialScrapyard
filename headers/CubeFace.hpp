#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "definitions.hpp"
#include <vector>
#include <memory>
#include "PerlinNoise/PerlinNoise.hpp"
#include "Noise.hpp"
#include "../headers/texture.hpp"

class CubeFace
{
private:
    const int RESOLUTION;
    //vertex position stored interleaved with vertex normal -> two vec3
    const int NUM_VERTICES;
    const int NUM_INDICES;
    const int NUM_EDGE_VERTICES;

    int computeNumVertices();
    int computeNumIndices();
    int computeNumEdgeVertices();

    glm::vec3 *vertices;
    int *indices;
    std::vector<float> displacements;
    std::vector<int> edgeVertexIndices;

    Noise *noise;

    GLuint VAO;
    GLuint VBO;
    glm::vec3 localUp;
    glm::vec3 axisA;
    glm::vec3 axisB;

    glm::vec3 getNormalForVertex(glm::vec3 vertex);
    void computeNormals();
public:
    CubeFace(glm::vec3 localUp, Noise &noise, int resolution);
    void uploadToGPU();
    void updateGPUBuffer();
    void addEdgeNormals(std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> &cubefaces);
    void draw();
    void drawInstanced(int instanceCount);
    bool addCrater(glm::vec3 center);
    std::tuple<glm::vec3, float> displacePointOnUnitSphere(glm::vec3 pointOnUnitSphere);
    std::vector<glm::vec3> filter_vertices_from_map();
    std::vector<glm::vec3> filter_vertices_and_normals_from_map(Noise &noise);
    ~ CubeFace();
};
#endif

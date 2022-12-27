#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "definitions.hpp"
#include <vector>
#include <memory>
#include "PerlinNoise/PerlinNoise.hpp"
#include "Noise.hpp"
#include "texture.hpp"

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
    ~ CubeFace();
};
#endif

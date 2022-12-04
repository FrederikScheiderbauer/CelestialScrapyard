#ifndef CUBEFACE_H
#define CUBEFACE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "definitions.hpp"
#include <vector>
#include <memory>
#include "PerlinNoise/PerlinNoise.hpp"
#include "Noise.hpp"

#define RESOLUTION 200
//vertex position stored interleaved with vertex normal -> two vec3
#define NUM_VERTICES RESOLUTION * RESOLUTION * 2
#define NUM_INDICES (RESOLUTION - 1) * (RESOLUTION - 1) * 6
#define NUM_EDGE_VERTICES 4 * RESOLUTION - 4

class CubeFace
{
private:
    glm::vec3 *vertices;
    int *indices;
    std::array<int, NUM_EDGE_VERTICES> edgeVertexIndices;

    std::vector<GLuint> texture_IDs;
    GLuint texture_Array_ID;
    GLuint VAO;
    glm::vec3 localUp;
    glm::vec3 axisA;
    glm::vec3 axisB;

    glm::vec3 getNormalForVertex(glm::vec3 vertex);
public:
    CubeFace(glm::vec3 localUp, Noise &noise, GLuint texture_Array_ID,std::vector<GLuint> _texture_IDs);
    void uploadToGPU();
    void addEdgeNormals(std::array<std::unique_ptr<CubeFace>, CUBE_NUM_FACES> &cubefaces);
    void draw();
    ~ CubeFace();
};
#endif

#include "CubeFace.hpp"
#include "glm/gtc/epsilon.hpp"
#include <math.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cstring>

glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b) {
    return glm::cross(a,b);
}

//http://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html
glm::vec3 computePointOnSphere(glm::vec3 pointOnCube) {
    glm::vec3 squared = glm::pow(pointOnCube, glm::vec3(2.f));
    float x = pointOnCube.x * glm::sqrt(1.f - squared.y/2.f - squared.z/2.f + (squared.y * squared.z)/3.f);
    float y = pointOnCube.y * glm::sqrt(1.f - squared.z/2.f - squared.x/2.f + (squared.z * squared.x)/3.f);
    float z = pointOnCube.z * glm::sqrt(1.f - squared.x/2.f - squared.y/2.f + (squared.x * squared.y)/3.f);
    return glm::vec3(x,y,z);
}

//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs
CubeFace::CubeFace(glm::vec3 localUp, Noise &noise,std::vector<Texture> _textures) {
    textures = _textures;
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    vertices = new glm::vec3[NUM_VERTICES] {};
    indices = new int[NUM_INDICES];
    int triangleIndex = 0;
    int edgeVertexIndicesIndex = 0;

    //compute vertices
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = computePointOnSphere(pointOnUnitCube);

            //elevation as in https://www.youtube.com/watch?v=uY9PAcNMu8s
            float displacement = noise.getValue(pointOnUnitSphere);
            pointOnUnitSphere = (1 + displacement) * pointOnUnitSphere;

            //set least significant bit of pointOnUnitSphere.x to zero to encode that this vertex is not within a crater
            uint32_t t;
            std::memcpy(&t, &pointOnUnitSphere.x, sizeof(i));
            t &= 0xFFFFFFFE;
            std::memcpy(&pointOnUnitSphere.x, &t, sizeof(i));

            int vertexIndex = i * 2;
            vertices[vertexIndex] = pointOnUnitSphere;
            displacements[i] = displacement;
            if (x == 0 || y == 0 || x == RESOLUTION - 1 || y == RESOLUTION - 1) {
                edgeVertexIndices[edgeVertexIndicesIndex] = vertexIndex;
                ++edgeVertexIndicesIndex;
            }

            if (x != RESOLUTION - 1 && y != RESOLUTION - 1) {
                //OpenGL uses stride provided by vertex attributes -> does not need to be multiplied by 2 for normal
                indices[triangleIndex] = i;
                indices[triangleIndex + 1] = i + RESOLUTION + 1;
                indices[triangleIndex + 2] = i + RESOLUTION;

                indices[triangleIndex + 3] = i;
                indices[triangleIndex + 4] = i + 1;
                indices[triangleIndex + 5] = i + RESOLUTION + 1;
                triangleIndex += 6;
            }
        }
    }
    computeNormals();
}

void CubeFace::computeNormals() {
    for (int j = 0; j < NUM_INDICES; j += 6) {
        //set normals to zero
        for (int k = 0; k < 6; ++k) {
            vertices[indices[j] * 2 + 1] = glm::vec3(0.f);
        }

        //compute triangle edges
        glm::vec3 edgeA1 = vertices[indices[j + 1] * 2] - vertices[indices[j] * 2];
        glm::vec3 edgeB1 = vertices[indices[j + 2] * 2] - vertices[indices[j] * 2];

        glm::vec3 edgeA2 = vertices[indices[j + 4] * 2] - vertices[indices[j] * 2];
        glm::vec3 edgeB2 = vertices[indices[j + 5] * 2] - vertices[indices[j] * 2];

        glm::vec3 normal1 = computeNormal(edgeA1, edgeB1);
        for (int k = 0; k < 3; ++k) {
            int normalIndex = indices[j + k] * 2 + 1;
            vertices[normalIndex] += normal1;
        }

        glm::vec3 normal2 = computeNormal(edgeA2, edgeB2);
        for (int k = 3; k < 6; ++k) {
            int normalIndex = indices[j + k] * 2 + 1;
            vertices[normalIndex] += normal2;
        }
    }
}

void CubeFace::addEdgeNormals(std::array<std::unique_ptr<CubeFace>, 6> &cubefaces) {
    for(int edgeVertexIndex : edgeVertexIndices) {
        glm::vec3 vertex = vertices[edgeVertexIndex];
        glm::vec3 normal = vertices[edgeVertexIndex + 1];
        for (int numFaces = 0; numFaces < CUBE_NUM_FACES; ++numFaces) {
            normal += cubefaces[numFaces]->getNormalForVertex(vertex);
        }
        vertices[edgeVertexIndex + 1] = normal;
    }
}

glm::vec3 CubeFace::getNormalForVertex(glm::vec3 vertex) {
    for(int edgeVertexIndex : edgeVertexIndices) {
        if (glm::all(glm::epsilonEqual(vertices[edgeVertexIndex], vertex, 1E-5f))) {
            //returns normal
            return vertices[edgeVertexIndex + 1];
        }
    }
    return glm::vec3(0.f);
}

void CubeFace::uploadToGPU() {
    //https://learnopengl.com/Getting-started/Hello-Triangle
    GLuint EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTICES * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES * sizeof(int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void CubeFace::activate_textures(){
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,textures[0].id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,textures[1].id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,textures[2].id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,textures[3].id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D,textures[4].id);
}


void CubeFace::draw() {
    glBindVertexArray(VAO);
 // just one texture ID for now, make dynamically select later TODO
    activate_textures();

    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

CubeFace::~CubeFace() {
    delete[] vertices;
    delete[] indices;
}

bool CubeFace::addCrater(glm::vec3 center) {
    float craterRadius = 0.2f;
    float craterDepth = 0.4f;
    //if all vertices are unchanged no update for the GPU is required
    bool changed = false;
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            int vertexIndex = i * 2;

            glm::vec3 pointOnUnitSphere = vertices[vertexIndex];
            float displacement = displacements[i];

            pointOnUnitSphere = pointOnUnitSphere / (1 + displacement);

            float distanceFromCenter = glm::length(pointOnUnitSphere - center);
            if (distanceFromCenter < craterRadius) {
                float craterHeight = craterDepth * glm::pow((distanceFromCenter * (1/craterRadius)), 2) - craterDepth;

                float newDisplacement = displacement + craterHeight;
                pointOnUnitSphere = (1 + newDisplacement) * pointOnUnitSphere;

                //set least significant bit of pointOnUnitSphere.x to one to encode that this vertex is within a crater
                uint32_t t;
                std::memcpy(&t, &pointOnUnitSphere.x, sizeof(i));
                t |= 1u;
                std::memcpy(&pointOnUnitSphere.x, &t, sizeof(i));

                vertices[vertexIndex] = pointOnUnitSphere;
                displacements[i] = displacement;

                changed = true;
            }
        }
    }

    if(changed) {
        computeNormals();
    }
    return changed;
}

void CubeFace::updateGPUBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_VERTICES * sizeof(glm::vec3), vertices);
}

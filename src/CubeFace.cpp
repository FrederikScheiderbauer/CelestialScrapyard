#include "../headers/CubeFace.hpp"
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

int CubeFace::computeNumVertices() {
    return RESOLUTION * RESOLUTION * 2;
}

int CubeFace::computeNumIndices() {
    return (RESOLUTION - 1) * (RESOLUTION - 1) * 6;
}

int CubeFace::computeNumEdgeVertices() {
    return 4 * RESOLUTION - 4;
}

std::tuple<glm::vec3, float> CubeFace::displacePointOnUnitSphere(glm::vec3 pointOnUnitSphere) {
    //elevation as in https://www.youtube.com/watch?v=uY9PAcNMu8s
    float displacement = noise->getValue(pointOnUnitSphere);
    return {(1 + displacement) * pointOnUnitSphere, displacement};
}

//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs
CubeFace::CubeFace(glm::vec3 localUp, Noise &noise, int resolution)
    : RESOLUTION(resolution), NUM_VERTICES(computeNumVertices()), NUM_INDICES(computeNumIndices()), NUM_EDGE_VERTICES(computeNumEdgeVertices()) {
    this->noise = &noise;
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    vertices = new glm::vec3[NUM_VERTICES] {};
    indices = new int[NUM_INDICES];
    displacements.reserve(NUM_VERTICES / 2);
    edgeVertexIndices.reserve(NUM_EDGE_VERTICES);
    int triangleIndex = 0;
    int edgeVertexIndicesIndex = 0;

    //compute vertices
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = computePointOnSphere(pointOnUnitCube);

            auto [point, displacement] = displacePointOnUnitSphere(pointOnUnitSphere);
            pointOnUnitSphere = point;

            //set least significant bit of pointOnUnitSphere.x to zero to encode that this vertex is not within a crater
            uint32_t t;
            std::memcpy(&t, &pointOnUnitSphere.x, sizeof(i));
            t &= 0xFFFFFFFE;
            std::memcpy(&pointOnUnitSphere.x, &t, sizeof(i));

            int vertexIndex = i * 2;
            vertices[vertexIndex] = pointOnUnitSphere;
            displacements.push_back(displacement);
            if (x == 0 || y == 0 || x == RESOLUTION - 1 || y == RESOLUTION - 1) {
                edgeVertexIndices.push_back(vertexIndex);
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

CubeFace::CubeFace(glm::vec3 localUp, int resolution)
    : RESOLUTION(resolution), NUM_VERTICES(computeNumVertices()), NUM_INDICES(computeNumIndices()), NUM_EDGE_VERTICES(computeNumEdgeVertices()) {
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    vertices = new glm::vec3[NUM_VERTICES] {};
    indices = new int[NUM_INDICES];
    displacements.reserve(NUM_VERTICES / 2);
    edgeVertexIndices.reserve(NUM_EDGE_VERTICES);
    int triangleIndex = 0;
    int edgeVertexIndicesIndex = 0;

    //compute vertices
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = computePointOnSphere(pointOnUnitCube);
            float displacement = 0.0f;

            //set least significant bit of pointOnUnitSphere.x to zero to encode that this vertex is not within a crater
            uint32_t t;
            std::memcpy(&t, &pointOnUnitSphere.x, sizeof(i));
            t &= 0xFFFFFFFE;
            std::memcpy(&pointOnUnitSphere.x, &t, sizeof(i));

            int vertexIndex = i * 2;
            vertices[vertexIndex] = pointOnUnitSphere;
            displacements.push_back(displacement);
            if (x == 0 || y == 0 || x == RESOLUTION - 1 || y == RESOLUTION - 1) {
                edgeVertexIndices.push_back(vertexIndex);
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


void CubeFace::draw() {
    glBindVertexArray(VAO);
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

void CubeFace::drawInstanced(int instanceCount) {
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, nullptr, instanceCount);
    glBindVertexArray(0);
}

std::vector<glm::vec3> CubeFace::filter_vertices_from_map() {
    std::vector<glm::vec3> results;
    /*
    for(int i = 0;  i < NUM_INDICES;i++) {
        
        int NUM_VALUES_PER_VERTEX = 2;
        int vertex_index = indices[i] * NUM_VALUES_PER_VERTEX;
        glm::vec3 vertex = vertices[vertex_index];
        float vertex_length = glm::length(vertex);
        float vertex_height = vertex.y;
        if(vertex_height > 1.3f) {
            results.push_back(vertex);
        }
    
    }
    */
    int middle = NUM_VERTICES/4;
    //int index = indices[middle];
    glm::vec3 middle_vertex = vertices[middle+50]*2.0f;// + displacements[middle/2];
    std::vector<glm::vec3> results_2;
    results.push_back(middle_vertex);
    return results;
}

std::vector<glm::vec3> CubeFace::filter_vertices_and_normals_from_map( Noise &noise) {
    std::vector<glm::vec3> results;
    for (int index = 0; index < NUM_INDICES; index+=2) {
        int vertex_index  = indices[index];
        glm::vec3 position_vertex = vertices[vertex_index];// + displacements[middle/2];
        glm::vec3 normal_vertex = vertices[vertex_index];
        float tree_chance = noise.getValue(position_vertex) * glm::length(position_vertex);
        if(tree_chance < 0.7f) {
            results.push_back(position_vertex);
            results.push_back(normal_vertex);
        }

    }
    return results;
}

std::vector<glm::vec3> CubeFace::get_Surface_Vertices_On_Surface(glm::vec3 center, float radius) {
    std::vector<glm::vec3> results;
    for ( int i = 0; i < NUM_VERTICES; i+=2) {
        glm::vec3 position_vertex = vertices[i];// + displacements[middle/2];
        glm::vec3 normal_vertex = vertices[i];
        if(glm::length(position_vertex-center) <= radius) {
            results.push_back(position_vertex);
            results.push_back(normal_vertex);
        }
    }
    return results;
}

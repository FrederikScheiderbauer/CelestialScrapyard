#include "CubeFace.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <iostream>

glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b) {
    return glm::cross(a,b);
}

//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs
CubeFace::CubeFace(glm::vec3 localUp, siv::PerlinNoise &perlin) {
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    glm::vec3 *vertices = new glm::vec3[NUM_VERTICES] {};
    int *indices = new int[NUM_INDICES];
    int triangleIndex = 0;

    //compute vertices
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

            float displacement = perlin.normalizedOctave3D_01(pointOnUnitSphere.x, pointOnUnitSphere.y, pointOnUnitSphere.z, 4);
            //map from [0,1] to [0.6, 1.4]
            displacement = (0.8f * displacement) + 0.6;
            pointOnUnitSphere = displacement * pointOnUnitSphere;

            vertices[i * 2] = pointOnUnitSphere;

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

    //compute normals, TODO: consider vertices on the edge of other faces
    for (int j = 0; j < NUM_INDICES; j += 6) {
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

    //https://learnopengl.com/Getting-started/Hello-Triangle
    GLuint VBO, EBO;
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

    delete[] vertices;
    delete[] indices;
}

void CubeFace::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#include "CubeFace.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>

glm::vec2 vertexToSpherical(glm::vec3 vertexPos) {
    //https://mathworld.wolfram.com/SphericalCoordinates.html
    float phi = glm::acos(vertexPos.z);
    float theta = glm::atan(vertexPos.y / vertexPos.x);
    return glm::vec2(phi / M_PI, theta / (2 * M_PI));
}

//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs
CubeFace::CubeFace(glm::vec3 localUp, std::vector<double> &heightmap) {
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    glm::vec3 vertices[NUM_VERTICES];
    int indices[NUM_INDICES];
    int triangleIndex = 0;

    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = x + y * RESOLUTION;
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

            glm::vec2 heightmapCoords = vertexToSpherical(pointOnUnitSphere);
            int heightmap_x = HEIGHTMAP_WIDTH * heightmapCoords.x;
            int heightmap_y = HEIGHTMAP_HEIGHT * heightmapCoords.y;
            float displacement = heightmap[heightmap_x * HEIGHTMAP_WIDTH + heightmap_y];
            pointOnUnitSphere = displacement * pointOnUnitSphere;

            vertices[i] = pointOnUnitSphere;

            if (x != RESOLUTION - 1 && y != RESOLUTION - 1) {
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

    //https://learnopengl.com/Getting-started/Hello-Triangle
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void CubeFace::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

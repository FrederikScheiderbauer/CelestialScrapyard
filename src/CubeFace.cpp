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

class Triangle {
private:
    glm::vec3 a;
    glm::vec3 b;
public:
    Triangle(glm::vec3 a, glm::vec3 b)
        : a{a}, b{b} {}
    glm::vec3 computeNormal() {
        return glm::cross(a,b);
    }
};

//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs
CubeFace::CubeFace(glm::vec3 localUp, std::vector<double> &heightmap) {
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    glm::vec3 vertices[NUM_VERTICES * 2] = {};
    int indices[NUM_INDICES];
    int triangleIndex = 0;

    //compute vertices
    for (int y = 0; y < RESOLUTION; y++) {
        for (int x = 0; x < RESOLUTION; x++) {
            int i = (x + y * RESOLUTION);
            glm::vec2 percent = glm::vec2(x, y) / (RESOLUTION - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);

            glm::vec2 heightmapCoords = vertexToSpherical(pointOnUnitSphere);
            int heightmap_x = HEIGHTMAP_WIDTH * heightmapCoords.x;
            int heightmap_y = HEIGHTMAP_HEIGHT * heightmapCoords.y;
            float displacement = heightmap[heightmap_x * HEIGHTMAP_WIDTH + heightmap_y];
            pointOnUnitSphere = displacement * pointOnUnitSphere;

            vertices[i * 2] = pointOnUnitSphere;

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

    //compute normals, TODO: consider vertices on the edge of other faces
    for (int j = 0; j < NUM_INDICES; j += 6) {
        Triangle triangle1{vertices[indices[j + 1] * 2] - vertices[indices[j] * 2], vertices[indices[j + 2] * 2] - vertices[indices[j] * 2]};
        Triangle triangle2{vertices[indices[j + 4] * 2] - vertices[indices[j] * 2], vertices[indices[j + 5] * 2] - vertices[indices[j] * 2]};

        glm::vec3 normal1 = triangle1.computeNormal();
        for (int k = 0; k < 3; ++k) {
            int normalIndex = indices[j + k] * 2 + 1;
            vertices[normalIndex] += normal1;
        }

        glm::vec3 normal2 = triangle1.computeNormal();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

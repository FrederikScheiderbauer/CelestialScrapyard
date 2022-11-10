#include "CubeFace.hpp"
//based on: https://github.com/SebLague/Procedural-Planets/blob/master/Procedural%20Planet%20E01/Assets/TerrainFace.cs

// constexpr int calculateNumVertices(int resolution) {
//     return resolution * resolution;
// }
//
constexpr int calculateNumIndices(int resolution) {
    return (resolution - 1) * (resolution - 1) * 6;
}

CubeFace::CubeFace(glm::vec3 localUp) {
    axisA = glm::vec3(localUp.y, localUp.z, localUp.x);
    axisB = glm::cross(localUp, axisA);

    //TODO: replace hardcoded resolution
    glm::vec3 vertices[100];
    int indices[486];
    int triangleIndex = 0;

    for (int y = 0; y < resolution; y++) {
        for (int x = 0; x < resolution; x++) {
            int i = x + y * resolution;
            glm::vec2 percent = glm::vec2(x, y) / (resolution - 1.0f);
            glm::vec3 pointOnUnitCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnUnitCube);
            vertices[i] = pointOnUnitSphere;

            if (x != resolution - 1 && y != resolution - 1) {
                indices[triangleIndex] = i;
                indices[triangleIndex + 1] = i + resolution + 1;
                indices[triangleIndex + 2] = i + resolution;

                indices[triangleIndex + 3] = i;
                indices[triangleIndex + 4] = i + 1;
                indices[triangleIndex + 5] = i + resolution + 1;
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
    glDrawElements(GL_TRIANGLES, calculateNumIndices(resolution), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

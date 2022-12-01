#include "CubeFace.hpp"
#include <math.h>
#include <glm/glm.hpp>

glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b) {
    return glm::cross(a,b);
}

float cavityShape(float x) {
    return x * x - 1;
}

float rimShape(float x, float rimWidth, float rimSteepness) {
    x = std::abs(x) - 1 - rimWidth;
    return rimSteepness * x * x;
}

float smoothMin(float a, float b, float k) {
    float h = glm::clamp((b - a + k) / (2 * k), 0.f, 1.f);
    return a * h + b * (1 - h) - k * h * (1 - h);
}

float computeCraterShape(float x) {
    float rimWidth = 0.7;
    float rimSteepness = 0.42;
    float floorHeight = -1.f;
    return smoothMin(smoothMin(cavityShape(x), rimShape(x, rimWidth, rimSteepness), 3), floorHeight, -3);
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
CubeFace::CubeFace(glm::vec3 localUp, siv::PerlinNoise &perlin,GLuint _texture_Array_ID,std::vector<GLuint> _texture_IDs) {
    texture_Array_ID = _texture_Array_ID;
    texture_IDs = _texture_IDs;
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
            glm::vec3 pointOnUnitSphere = computePointOnSphere(pointOnUnitCube);

            float displacement = perlin.normalizedOctave3D_01(pointOnUnitSphere.x, pointOnUnitSphere.y, pointOnUnitSphere.z, 8, 0.5f);
            //map from [0,1] to [0.6, 1.4]
            displacement = (0.8f * displacement) + 0.6;

            pointOnUnitSphere = displacement * pointOnUnitSphere;

            glm::vec3 craterCenter = localUp;
            float craterRadius = 0.2;
            float craterShape = computeCraterShape(glm::length(pointOnUnitSphere - craterCenter) / craterRadius);
            float craterHeight = craterShape * craterRadius;

            //pointOnUnitSphere = pointOnUnitSphere * craterHeight;

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY,texture_Array_ID); // just one texture ID for now, make dynamically select later TODO
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture_IDs[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texture_IDs[1]);

    glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

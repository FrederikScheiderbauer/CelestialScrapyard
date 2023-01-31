//Shadow Mapping based on: //https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
#include "../headers/LightSource.hpp"
#include "../headers/OrbitUtil.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

LightSource::LightSource() {
    position = getRandomPositionInOrbit(10.f, 10.1f);

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    allocateTextures();
    //allocateTexturesMultisample();
}

void LightSource::allocateTexturesMultisample() {
    int number_samples = 4;
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthMap);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,number_samples, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void LightSource::allocateTextures() {
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void LightSource::updatePosition() {
    position = moveInOrbit(position, LIGHT_SOURCE_SPEED, false);
}

void LightSource::bindToShader(GLuint program) {
    glUniform3fv(glGetUniformLocation(program, "lightDir"), 1, &position[0]);
}

LightSource& LightSource::getInstance() {
    static LightSource instance;
    return instance;
}

void LightSource::prepareDepthMapCreation() {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);
}
void LightSource::prepareDepthMapCreationMultisample() {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void LightSource::finishDepthMapCreation(int width, int height) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightSource::finishDepthMapCreationMultisample(int width, int height) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightSource::bindLightMatrices(GLuint program) {
    float near_plane = 5.f, far_plane = 20.f;
    glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -2.5f, 2.5f, near_plane ,far_plane);
    glm::mat4 lightView = glm::lookAt(position, glm::vec3( 0.0f, 0.0f,  0.0f),glm::vec3( 0.0f, 1.0f,  0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

GLuint LightSource::getDepthMap() {
    return depthMap;
}

void LightSource::bindDepthMap() {
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

void LightSource::bindDepthMapMultisample() {
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthMap);
}

glm::vec3 &LightSource::getPosition() {
    return position;
}

#include "skybox.hpp"
#include <vector>
#include <string>
#include <config/config.h>
#include <GLFW/glfw3.h>
#include "texture_loader.hpp"
#include <iostream>



std::vector<std::string> skyboxPaths = {
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/right.png",
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/left.png",
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/top.png",
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/bot.png",
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/front.png",
    (std::string)Project_SOURCE_DIR +"/src/assets/skybox/back.png",
};
const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/skybox.vert", (std::string)Project_SOURCE_DIR + "/src/shader/skybox.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
//https://learnopengl.com/Advanced-OpenGL/Cubemaps
Skybox::Skybox()
{
    skyboxShader = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };


    unsigned int skyboxVBO;
    glGenVertexArrays(1,&skyboxVAO);
    glGenBuffers(1,&skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER,skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);


    skyboxTexture = TextureLoader::generate_cubemap(skyboxPaths);
    skyboxShader->use(); // don't forget to activate the shader before setting uniforms! 
    glUniform1i(glGetUniformLocation(skyboxShader->name, "skybox"), 0); // set it manually
}


void Skybox::draw(int width, int height)
{
    LockedCamera* camera = LockedCamera::get_Active_Camera();
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();

    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = camera->get_Camera_Position();
    view = glm::mat4(glm::mat3(glm::lookAt(cameraPos,camera->get_Camera_Target(),camera->get_Camera_Up())));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->name, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader->name, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,skyboxTexture);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

}

void Skybox::shutdown()
{
    glDeleteVertexArrays(1, &skyboxVAO);
}

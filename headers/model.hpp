#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "ShaderProgram.hpp"
#include <memory>
#include <array>

class Model{
    public:
        Model();
        virtual void draw(int width,int height) = 0;

    protected:
        unsigned int VAO, instanceVBO, instanceMatrixVBO;
        Mesh mesh;  
        std::vector<Material> object_materials;
        std::unique_ptr<ShaderProgram> modelShader;
        void setUniformMatrix(glm::mat4 matrix, std::string type);
};


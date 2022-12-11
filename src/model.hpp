#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "ShaderProgram/ShaderProgram.hpp"
#include <memory>
#include <array>

class Model{
    public:
        Model(std::string obj_file, std::string materials_directory);
        void draw(int width,int heigh);

    private:
        unsigned int VAO;
        std::unique_ptr<ShaderProgram> treeShader;
        std::vector<float> model_vertices;
        std::vector<unsigned int> model_vertices_indices;
        std::vector<glm::vec3> model_normals;
        std::vector<unsigned int> model_normals_indices;
        std::vector<Triangle> model_triangles;
        void setUniformMatrix(glm::mat4 matrix, std::string type);
};
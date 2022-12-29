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
        Model(std::string obj_file, std::string materials_directory);
        void draw(int width,int height);
        void draw_at_coordinates(float x, float y, float z, int width, int height);

    private:
        unsigned int VAO;
        Mesh mesh;
        std::vector<Material> object_materials;
        std::unique_ptr<ShaderProgram> modelShader;
        std::vector<unsigned int> model_vertices_indices;
        std::vector<glm::vec3> model_normals;
        std::vector<unsigned int> model_normals_indices;
        void setUniformMatrix(glm::mat4 matrix, std::string type);
};

class Tree : public Model {
    private:
        //Mesh
        //materials;
        //Textures & Colors;
        glm::vec3 position;//position
    public:
        //void draw(int width, int height) override;



};
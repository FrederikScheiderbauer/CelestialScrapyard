#pragma once

#include "model.hpp"

//std::string obj_file = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack/low_poly_tree_1.obj";
//std::string material_directory = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack";

class PineTree: public Model{
    private:
        std::vector<float> setupVertices(std::vector<glm::vec3> model_vertices,std::vector<glm::vec3> model_normals);
        void set_instance_buffer(std::vector<glm::vec3> offsets);
        void calculate_TreeOffsets();
        glm::vec2 translations[100];
        std::vector<glm::vec3> tree_offsets;
        

    public:
        PineTree();
        void draw(int width, int height) override;
        void draw_instanced(int width,int height,std::vector<glm::vec3>& tree_offsets_2);
};
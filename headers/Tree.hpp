#pragma once

#include "model.hpp"

std::string obj_file = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack/low_poly_tree_1.obj";
std::string material_directory = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack";

class PineTree{
    private:
        Model pineTreeModel;

    public:
        PineTree();
        void draw(int width,int height);
};
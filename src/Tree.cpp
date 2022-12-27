#include <string>
#include <config/config.h>


#include "../headers/model.hpp"
#include "../headers/Tree.hpp"


std::string obj_file = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack/low_poly_tree_1.obj";
std::string material_directory = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack";

PineTree::PineTree() {
    pineTreeModel = Model(obj_file,material_directory);
}

void PineTree::draw(int width,int height) {
    pineTreeModel.draw(width,height);

}

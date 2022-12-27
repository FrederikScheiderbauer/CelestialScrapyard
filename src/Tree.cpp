#include <string>
#include <config/config.h>


#include "../headers/model.hpp"
#include "../headers/Tree.hpp"


PineTree::PineTree() {
    pineTreeModel = Model(obj_file,material_directory);
}

void PineTree::draw(int width,int height) {
    pineTreeModel.draw(width,height);

}

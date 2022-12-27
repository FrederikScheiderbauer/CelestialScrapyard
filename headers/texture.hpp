#pragma once

#include <string>

enum Texture_Types {diffuse,specular,cubemap};
struct Texture {
    unsigned int id;
    std::string type;
};
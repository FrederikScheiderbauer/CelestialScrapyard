#pragma once

#include <string>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define TINYOBJLOADER_IMPLEMENTATION

struct Triangle {
  std::array<unsigned int,3> vertices; //stores vertices indices
  std::array<unsigned int,3> normals; //stores normals indices
  Triangle(std::array<unsigned int,3> v, std::array<unsigned int,3> n) : vertices(v),normals(n) {};
};

void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<Triangle> &triangles, std::vector<glm::vec3> & vertnormals);
void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<unsigned int> &vertex_indices, std::vector<glm::vec3> & vertnormals,std::vector<unsigned int> &normals_indices);
#pragma once

#include <string>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define TINYOBJLOADER_IMPLEMENTATION


struct Material {
  std::string name;
  unsigned int id;
  std::array<float,3> ambient;
  std::array<float,3> diffuse;
  std::array<float,3> specular;
  Material(std::string n,unsigned int i,std::array<float,3> a,std::array<float,3> d,std::array<float,3> s) : name(n),id(i),ambient(a),diffuse(d),specular(s) {};
};

struct Triangle {
  std::array<unsigned int,3> vertices; //stores vertices indices
  std::array<unsigned int,3> normals; //stores normals indices
  unsigned int material_id;
  
  Triangle(std::array<unsigned int,3> v, std::array<unsigned int,3> n, unsigned int m) : vertices(v),normals(n),material_id(m) {};
};
void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<Triangle> &triangles, std::vector<glm::vec3> & vertnormals);
void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<unsigned int> &vertex_indices, std::vector<glm::vec3> & vertnormals,std::vector<unsigned int> &normals_indices);
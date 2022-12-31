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

struct Mesh{
  std::vector<unsigned int> vertices_indices;
  std::vector<unsigned int> normals_indices;
  std::vector<unsigned int> uvs_indices;
  std::vector<int> material_indices;
 };
struct Triangle{
  std::array<int,3> vertices;
	std::array<int,3> normals;
	std::array<int,3> uvs;
	int material;
	Triangle(std::array<int,3> v, std::array<int,3> n, std::array<int,3> uv, int m) : vertices(v), normals(n), uvs(uv), material(m) {};
};


void load_obj(std::string obj_file,std::string materials_directory,std::vector<glm::vec3> &vertices, Mesh& mesh, std::vector<glm::vec3> & vertnormals, std::vector<glm::vec2>& vertuvs, std::vector<Material>& materials);
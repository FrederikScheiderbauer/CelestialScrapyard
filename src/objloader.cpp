#include "../headers/objloader.hpp"
#include "tinyobjloader/tiny_obj_loader.h"
#include "config/config.h"
#include <string>
#include <array>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

void load_mesh(std::vector<tinyobj::shape_t>& shapes, Mesh& object_mesh) {

  for(auto shape = shapes.begin(); shape < shapes.end(); ++shape) {
    std::vector<tinyobj::index_t> & indices = shape->mesh.indices;//shape->mesh.indices;
    std::vector<int> & mat_ids = shape->mesh.material_ids;
    std::cout << "Loading " << mat_ids.size() << " triangles..." << std::endl;

    for(size_t face_ind = 0; face_ind < mat_ids.size(); face_ind++) {
      object_mesh.vertices_indices.push_back(indices[3*face_ind].vertex_index);
      object_mesh.vertices_indices.push_back(indices[3*face_ind+1].vertex_index);
      object_mesh.vertices_indices.push_back(indices[3*face_ind+2].vertex_index);
      object_mesh.normals_indices.push_back(indices[3*face_ind].normal_index);
      object_mesh.normals_indices.push_back(indices[3*face_ind+1].normal_index);
      object_mesh.normals_indices.push_back(indices[3*face_ind+2].normal_index);
      object_mesh.uvs_indices.push_back(indices[3*face_ind].texcoord_index);
      object_mesh.uvs_indices.push_back(indices[3*face_ind+1].texcoord_index);
      object_mesh.uvs_indices.push_back(indices[3*face_ind+2].texcoord_index);
      object_mesh.material_indices.push_back(mat_ids[face_ind]);
      object_mesh.material_indices.push_back(mat_ids[face_ind]);
      object_mesh.material_indices.push_back(mat_ids[face_ind]);
      
    }
  }
}
void components_to_vec3s(const std::vector<float> components, std::vector<glm::vec3>& vecs) {
    //convert a vector of back-to-back vertex components to a vector of vec3 objects
    for(size_t vec_start = 0; vec_start < components.size(); vec_start+=3) {
        vecs.push_back(
            glm::vec3(components[vec_start],
                components[vec_start+1],
                components[vec_start+2]
            ));
    }
}
void components_to_vec2s(const std::vector<float> components, std::vector<glm::vec2>& vecs) {
    //convert a vector of back-to-back vertex components to a vector of vec2 objects
    for(size_t vec_start = 0; vec_start < components.size(); vec_start+=2) {
        vecs.push_back(
            glm::vec2(components[vec_start],
                components[vec_start+1]
            ));
    }
}
void load_materials(std::vector<tinyobj::material_t> objmaterials, std::vector<Material>& materials) {
  unsigned int index = 0;
  for ( auto material : objmaterials) { 
    std::array<float,3> ambient = {material.ambient[0],material.ambient[1],material.ambient[2]};
    std::array<float,3> diffuse = {material.diffuse[0],material.diffuse[1],material.diffuse[2]};
    std::array<float,3> specular = {material.specular[0],material.specular[1],material.specular[2]};
    materials.push_back(Material(material.name,index,ambient,diffuse,specular));
    index++;
    std::cout << "material added" << std::endl;
  }

}


//https://github.com/canmom/rasteriser
void load_obj(std::string obj_file,std::string materials_directory,std::vector<glm::vec3> &vertices, Mesh& mesh, std::vector<glm::vec3> & vertnormals, std::vector<glm::vec2>& vertuvs, std::vector<Material>& materials){
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> objmaterials;
  std::string warn;
  std::string err;

  bool success = tinyobj::LoadObj(&attrib, &shapes, &objmaterials, &warn, &err,
    obj_file.c_str(), //model to load
    materials_directory.c_str(), //directory to search for materials
    true);


  //error handling
  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
  if (!warn.empty()) {
    std::cerr << warn << std::endl;
  }
  if (!success) {
    exit(1);
  }

  //convert the vertices into our format
  components_to_vec3s(attrib.vertices, vertices);

  //convert the vertex normals into our format
  components_to_vec3s(attrib.normals, vertnormals);

  //convert the uv coordinates into our format
  if(attrib.texcoords.size() > 0) {
    components_to_vec2s(attrib.texcoords, vertuvs);
  }
  //convert materials and load textures
  load_materials(objmaterials, materials);

  load_mesh(shapes, mesh);
}
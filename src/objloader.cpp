#include "objloader.hpp"
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
/*
void load_triangles(const tinyobj::shape_t & shape, std::vector<Triangle> & triangles) {
    //convert a tinyobjloader shape_t object containing indices into vertex properties and textures
    //into a vector of Triangle objects grouping these indices
    const std::vector<tinyobj::index_t> & indices = shape.mesh.indices;
    const std::vector<int> & mat_ids = shape.mesh.material_ids;

    std::cout << "Loading " << mat_ids.size() << " triangles..." << std::endl;

    for(size_t face_ind = 0; face_ind < mat_ids.size(); face_ind++) {
        triangles.push_back(
            Triangle(
                {indices[3*face_ind].vertex_index, indices[3*face_ind+1].vertex_index, indices[3*face_ind+2].vertex_index},
                {indices[3*face_ind].normal_index, indices[3*face_ind+1].normal_index, indices[3*face_ind+2].normal_index}
                ));
    }
}
*/

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
void components_to_vector(const std::vector<float> components, std::vector<float>& vecs) {
    //convert a vector of back-to-back vertex components to a vector of vec3 objects
    for(size_t index = 0; index < components.size(); index++) {
        vecs.push_back(components[index]);
    }
}

void indices_to_vector(const tinyobj::shape_t & shape, std::vector<unsigned int>& vector_indices, std::vector<unsigned int>& normals_indices) {
  const std::vector<tinyobj::index_t> & indices = shape.mesh.indices;
  const std::vector<int> & mat_ids = shape.mesh.material_ids;
  for(size_t index = 0; index < indices.size(); index++) { 
    vector_indices.push_back((unsigned int)indices[index].vertex_index);
    normals_indices.push_back((unsigned int)indices[index].normal_index);

  }
}

//https://github.com/canmom/rasteriser
void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<Triangle> &triangles, std::vector<glm::vec3> & vertnormals){
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
  //
    //convert the vertices into our format
    components_to_vector(attrib.vertices, vertices);

    //convert the vertex normals into our format
    components_to_vec3s(attrib.normals, vertnormals);
  /*
    for(auto shape = shapes.begin(); shape < shapes.end(); ++shape) {
      load_triangles(*shape, triangles);
    }
    */
}

void load_obj(std::string obj_file,std::string materials_directory,std::vector<float> &vertices, std::vector<unsigned int> &vertex_indices, std::vector<glm::vec3> & vertnormals,std::vector<unsigned int> &normals_indices){
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
  //
    //convert the vertices into our format
    components_to_vector(attrib.vertices, vertices);

    //convert the vertex normals into our format
    components_to_vec3s(attrib.normals, vertnormals);
  indices_to_vector(shapes[0],vertex_indices,normals_indices);

  /*
    for(auto shape = shapes.begin(); shape < shapes.end(); ++shape) {
      indices_to_vector(*shape,vertex_indices,normals_indices);
    }
  */
}
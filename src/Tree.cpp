#include <string>
#include <config/config.h>


#include "../headers/model.hpp"
#include "../headers/Tree.hpp"
#include "../headers/camera.hpp"
#include "../headers/LightSource.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp> 
#include <glm/gtc/quaternion.hpp>


std::string obj_file = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack/low_poly_tree_1.obj";
std::string material_directory = (std::string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack";

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/tree.vert", (std::string)Project_SOURCE_DIR + "/src/shader/tree.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

std::vector<float> PineTree::setupVertices(std::vector<glm::vec3> model_vertices,std::vector<glm::vec3> model_normals) {
    std::vector<float> result = std::vector<float>(7*model_vertices.size());// 7 comes from the datafromat chosen,which contains 7 float values per vertex
    for ( int i = 0; i < mesh.vertices_indices.size();i++) {
        int vertex_index = mesh.vertices_indices[i];
        int normal_index = mesh.normals_indices[i];
        int material_id = mesh.material_indices[i];

        result[vertex_index*7] = model_vertices[vertex_index].x;
        result[vertex_index*7+1] = model_vertices[vertex_index].y;
        result[vertex_index*7+2] = model_vertices[vertex_index].z;

        result[vertex_index*7+3] = model_normals[normal_index].x;
        result[vertex_index*7+4] = model_normals[normal_index].y;
        result[vertex_index*7+5] = model_normals[normal_index].z;

        result[vertex_index*7+6] = (float) material_id;
    }

    return result;
}
void PineTree::setupTreeData(std::vector<glm::vec3> model_vertices,std::vector<glm::vec3> model_normals,std::vector<float>& vertices,std::vector<float>& normals,std::vector<int>& material_ids) {
    //std::vector<float> result = std::vector<float>(7*model_vertices.size());// 7 comes from the datafromat chosen,which contains 7 float values per vertex
    for ( int i = 0; i < mesh.vertices_indices.size();i++) {
        int vertex_index = mesh.vertices_indices[i];
        int normal_index = mesh.normals_indices[i];
        int material_id = mesh.material_indices[i];

        vertices[vertex_index*3] = model_vertices[vertex_index].x;
        vertices[vertex_index*3+1] = model_vertices[vertex_index].y;
        vertices[vertex_index*3+2] = model_vertices[vertex_index].z;
        //vertices.push_back(model_vertices[vertex_index].x);
        //vertices.push_back(model_vertices[vertex_index].y);
        //vertices.push_back(model_vertices[vertex_index].z);

        normals[vertex_index*3] = model_normals[normal_index].x;
        normals[vertex_index*3+1] = model_normals[normal_index].y;
        normals[vertex_index*3+2] = model_normals[normal_index].z;
        //normals.push_back(model_normals[normal_index].x);
        //normals.push_back(model_normals[normal_index].y);
        //normals.push_back(model_normals[normal_index].z);

        material_ids[vertex_index] = material_id;
        //material_ids.push_back(material_id);
    }
}
/*
void CubeFace::updateGPUBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_VERTICES * sizeof(glm::vec3), vertices);
}
*/
void PineTree::set_instance_buffer(std::vector<glm::vec3>& offsets) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * offsets.size(), &offsets[0], GL_STATIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, tree_offsets.size() * sizeof(glm::vec3), &tree_offsets[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void PineTree::set_instance_matrix_buffer(std::vector<glm::mat4>& instanceMatrices) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceMatrices.size(), &instanceMatrices[0], GL_STATIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, tree_offsets.size() * sizeof(glm::vec3), &tree_offsets[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

PineTree::PineTree() {
    std::vector<glm::vec3> model_vertices;
    std::vector<glm::vec3> model_normals;
    modelShader = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    std::vector<glm::vec2> vertuvs;

    load_obj(obj_file,material_directory,model_vertices,mesh,model_normals,vertuvs,object_materials);
    std::vector<float> vertices = setupVertices(model_vertices,model_normals);

    std::vector<float> vertices_2 = std::vector<float>(model_vertices.size()*3);
    std::vector<float> normals = std::vector<float>(model_vertices.size()*3);
    std::vector<int> material_ids= std::vector<int>(model_vertices.size());

    setupTreeData(model_vertices,model_normals,vertices_2,normals,material_ids);
    //setup VAO and bind vertices/normals/texcoords
    unsigned int VBO,EBO;
    unsigned int vertexVBO,normalVBO,materialVBO;


    //vertex format
    //vertices: v.x;v.y;v.z; ;n.x;n.y;n.z; ;m.id;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &instanceMatrixVBO);
    //glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &materialVBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER,7* sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int) * mesh.vertices_indices.size(), &mesh.vertices_indices[0], GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, materialVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * material_ids.size(), &material_ids[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_2.size(), &vertices_2[0], GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), &normals[0], GL_STATIC_DRAW);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, materialVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * material_ids.size(), &material_ids[0], GL_STATIC_DRAW);
    //glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,7 * sizeof(float), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE,sizeof(int), (void*)0);
    
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::mat4),NULL,GL_STATIC_DRAW);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    
    
    
    
}

void PineTree::draw(int width,int height) {
    Camera* camera = Camera::get_Active_Camera();
    modelShader->use();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(1.0f);
    setUniformMatrix(model,"model");

    glm::mat4 view = glm::mat4(1.0f);

    view = camera->get_View_Matrix();

    setUniformMatrix(view,"view");
    
    modelShader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.vertices_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::vector<glm::mat4> calculate_tree_transformations(std::vector<glm::vec3>& offsets){
    //offsets2 consist of vec3s which apepar in pairs: 0:pos_vector;1:normal_vector...n-2:pos_vector;n-1:normal_vector
    std::vector<glm::mat4> model_matrices;
    float tree_scaling = 0.2f;
    for(int i = 0; i< offsets.size();i +=2) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 model_pos_offset = offsets[i];
        glm::vec3 model_normal_offset = offsets[i+1];

        model = glm::translate(model,model_pos_offset);

        model = glm::scale(model,glm::vec3(tree_scaling));

        glm::vec3 localUp = glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 origin = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 direction = glm::normalize(model_pos_offset);

        glm::vec3 rotAxis = glm::normalize(glm::cross(localUp,(model_normal_offset)));
        float rotAngle = glm::acos(glm::dot(localUp,model_normal_offset));

        model = glm::rotate(model,rotAngle,rotAxis);
        model_matrices.push_back(model);
    }
    return model_matrices;
}

void PineTree::draw_instanced(int width,int height,std::vector<glm::mat4> instance_matrices, glm::vec3& planet_info) {
    //std::vector<glm::mat4> model_transformations = calculate_tree_transformations(tree_offsets_2);
    set_instance_matrix_buffer(instance_matrices);
    //set_instance_buffer(tree_offsets_2);
    Camera* camera = Camera::get_Active_Camera();
    //int amount = tree_offsets_2.size();
    int amount = instance_matrices.size();
    modelShader->use();
    //remove later TODO

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    setUniformMatrix(proj,"projection");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model,glm::vec3(0.001f));
    setUniformMatrix(model,"model");

    glm::mat4 view = glm::mat4(1.0f);

    view = camera->get_View_Matrix();

    setUniformMatrix(view,"view");
    glm::vec3 cameraPos = camera->get_Camera_Position();
    glUniform3fv(glGetUniformLocation(modelShader->name, "cameraPos"), 1, &cameraPos[0]);
    LightSource::getInstance().bindToShader(modelShader->name);

    glUniform3fv(glGetUniformLocation(modelShader->name, "planet_info"), 1, &planet_info[0]);
    glUniform1i(glGetUniformLocation(modelShader->name, "depthRender"), false);
    modelShader->use();
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.vertices_indices.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);

}

void PineTree::draw_for_depth_map(std::vector<glm::mat4> instance_matrices, glm::vec3 &planet_info) {
    modelShader->use();
    set_instance_matrix_buffer(instance_matrices);

    glUniform1i(glGetUniformLocation(modelShader->name, "depthRender"), true);
    LightSource::getInstance().bindLightMatrices(modelShader->name);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model,glm::vec3(0.001f));
    setUniformMatrix(model,"model");
    glUniform3fv(glGetUniformLocation(modelShader->name, "planet_info"), 1, &planet_info[0]);

    int amount = instance_matrices.size();
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.vertices_indices.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}


#include <string>
#include <config/config.h>


#include "../headers/model.hpp"
#include "../headers/Tree.hpp"
#include "../headers/camera.hpp"
#include <iostream>


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
        /*
        if(!result[vertex_index*7]) {
            result[vertex_index*7] = model_vertices[vertex_index].x;
            
            result[vertex_index*7+1] = model_vertices[vertex_index].y;
            result[vertex_index*7+2] = model_vertices[vertex_index].z;

            result[vertex_index*7+3] = model_normals[normal_index].x;
            result[vertex_index*7+4] = model_normals[normal_index].y;
            result[vertex_index*7+5] = model_normals[normal_index].z;

            result[vertex_index*7+6] = (float) material_id;
            
        }
        */

    }

    return result;
}

std::vector<int> setupIndices() {
    std::vector<int> result;
    return result;
}

PineTree::PineTree() {
    std::vector<glm::vec3> model_vertices;
    std::vector<glm::vec3> model_normals;
    modelShader = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);
    std::vector<glm::vec2> vertuvs;

    load_obj(obj_file,material_directory,model_vertices,mesh,model_normals,vertuvs,object_materials);
    std::vector<float> vertices = setupVertices(model_vertices,model_normals);
    //setup VAO and bind vertices/normals/texcoords
    unsigned int VBO,EBO;



    //vertex format
    //vertices: v.x;v.y;v.z; ;n.x;n.y;n.z; ;m.id;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,7* sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    //glBufferData(GL_ARRAY_BUFFER,6* sizeof(float) * model_vertices.size(), &model_vertices[0], GL_STATIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER,3*sizeof(float) * model_vertices.size(),model_normals.size(),&model_normals[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * mesh.vertices_indices.size(), &mesh.vertices_indices[0], GL_STATIC_DRAW);

    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.vertices_indices.size() + sizeof(unsigned int) * mesh.normals_indices.size(), &mesh.vertices_indices[0], GL_STATIC_DRAW);
    //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * mesh.vertices_indices.size(),mesh.normals_indices.size(),&mesh.normals_indices[0]);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,7* sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
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

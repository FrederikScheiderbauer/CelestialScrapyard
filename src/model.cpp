#include "../headers/model.hpp"
#include <GLFW/glfw3.h>
#include <config/config.h>
#include "../headers/camera.hpp"

const std::vector<std::string> SHADER_PATHS = {(std::string)Project_SOURCE_DIR +"/src/shader/tree.vert", (std::string)Project_SOURCE_DIR + "/src/shader/tree.frag"};

const std::vector<GLenum> SHADER_TYPES = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};


Model::Model() {
    
}
Model::Model(std::string obj_file, std::string materials_directory){

    std::vector<float> model_vertices;
    modelShader = std::make_unique<ShaderProgram>(SHADER_PATHS, SHADER_TYPES);

    load_obj(obj_file,materials_directory,model_vertices,model_vertices_indices,model_normals,model_normals_indices);



    unsigned int VBO,EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_vertices.size(), &model_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * model_vertices_indices.size(), &model_vertices_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

}

void Model::setUniformMatrix(glm::mat4 matrix, std::string type)
{
    glUniformMatrix4fv(glGetUniformLocation(modelShader->name, type.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Model::draw_at_coordinates(float x, float y, float z, int width, int height) {

}


void Model::draw(int width,int height) {
    
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
    glDrawElements(GL_TRIANGLES, model_vertices_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
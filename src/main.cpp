#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>

#include <tinyobjloader/tiny_obj_loader.h>
#include "skybox.hpp"
#include "gui.hpp"
#include "Sphere.hpp"
#include "camera.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
//https://www.glfw.org/docs/3.3/input_guide.html
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto camera = LockedCamera::get_Active_Camera();
    camera->handle_scroll_event((float)yoffset);

}

//https://learnopengl.com/Getting-started/Hello-Window
void processInput(GLFWwindow *window, Sphere &sphere)
{
    auto camera = LockedCamera::get_Active_Camera();
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_W);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_S);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_A);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_D);
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glm::vec3 intersectionOnUnitSphere = glm::normalize(camera->get_Camera_Position());
        sphere.addCrater(intersectionOnUnitSphere);
    }
}

const int WIDTH = 512;
const int HEIGHT = 512;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, WIDTH, "Hello World", NULL, NULL);
    glm::vec3 first_camera_Position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 first_camera_Target = glm::vec3(0.0,0.0,0.0);
    float speed = 0.3f;

    LockedCamera camera = LockedCamera(first_camera_Position,first_camera_Target,speed);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Camera configurations for Menu purposes,till that part is done.
    bool is_Locked_Camera = true;
    bool is_Free_Camera = false;
    float current_Camera_Speed = 0.5f;
    //

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //https://stackoverflow.com/questions/58053885/having-an-issue-with-gladloadgl-im-getting-an-error-saying-it-does-not-take
    int version = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    glViewport(0,0,WIDTH,HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window,mouse_scroll_callback);

    Gui gui_Object;
    gui_Object.imgui_Init(window);

    //get time
    auto last_Time = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point current_Time;
    unsigned long seed = std::chrono::time_point_cast<std::chrono::nanoseconds>(last_Time).time_since_epoch().count();
    std::chrono::duration<double> elapsed_Time;
    bool is_Wireframe = false;
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Sphere sphere = Sphere(seed);
    Skybox skybox = Skybox();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        current_Time = std::chrono::system_clock::now();
        elapsed_Time = current_Time - last_Time;
        //std::cout << "Elapsed Time: "<< elapsed_Time.count() << endl;

        /*Input handling here*/
        processInput(window, sphere);


        /*Update Game state*/

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui_Object.imgui_Frame_Setup();

        int current_width, current_height;
        glfwGetWindowSize(window, &current_width, &current_height);

        sphere.draw(current_width, current_height);

        skybox.draw(current_width, current_height);// render skybox as last object in the scene, for optimization purposes.

        gui_Object.imgui_Camera_Control_Window(&is_Locked_Camera,&is_Free_Camera,&current_Camera_Speed);
        gui_Object.imgui_Debug_Window(&is_Wireframe);

        gui_Object.imgui_Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        last_Time = current_Time;
    }

    gui_Object.imgui_Shutdown();
    skybox.shutdown();
    glfwTerminate();
    return 0;
}

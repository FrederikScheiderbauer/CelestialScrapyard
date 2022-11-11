#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

#include "gui.hpp"
#include "Sphere.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


//https://learnopengl.com/Getting-started/Hello-Window
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

    //https://stackoverflow.com/questions/58053885/having-an-issue-with-gladloadgl-im-getting-an-error-saying-it-does-not-take
    int version = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    glViewport(0,0,WIDTH,HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Gui gui_Object;
    gui_Object.imgui_Init(window);

    Sphere sphere = Sphere();

    //get time
    auto last_Time = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point current_Time;
    std::chrono::duration<double> elapsed_Time;

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        current_Time = std::chrono::system_clock::now();
        elapsed_Time = current_Time - last_Time;
        //std::cout << "Elapsed Time: "<< elapsed_Time.count() << endl;

        /*Input handling here*/
        processInput(window);


        /*Update Game state*/

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gui_Object.imgui_Frame_Setup();

        sphere.rotate_Camera();
        sphere.draw();
        gui_Object.imgui_Camera_Control_Window(&is_Locked_Camera,&is_Free_Camera,&current_Camera_Speed);

        gui_Object.imgui_Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        last_Time = current_Time;
    }

    gui_Object.imgui_Shutdown();

    glfwTerminate();
    return 0;
}

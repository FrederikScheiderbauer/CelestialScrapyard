#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>

#include <tinyobjloader/tiny_obj_loader.h>
#include "../headers/skybox.hpp"
#include "../headers/gui.hpp"
#include "../headers/Planet.hpp"
#include "../headers/camera.hpp"
#include "../headers/objloader.hpp"
#include <config/config.h>
#include "../headers/model.hpp"
#include "../headers/AsteroidBelt.hpp"
#include "../headers/Tree.hpp"
#include "../headers/LightSource.hpp"

using namespace std;


//mouse configuration; TODO package nicer
bool firstMouse = true;
float lastX;
float lastY;

float deltaTime;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
//https://www.glfw.org/docs/3.3/input_guide.html
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto camera = LockedCamera::get_Active_Camera();
    camera->handle_scroll_event((float)xoffset,(float)yoffset,deltaTime);

}

struct WindowPointerParameters {
    Planet *planet;
    AsteroidBelt *belt;
    bool mouseClicked;
    glm::vec2 mousePosition;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        Planet *planet = ((WindowPointerParameters*) glfwGetWindowUserPointer(window))->planet;
        AsteroidBelt *belt = ((WindowPointerParameters*) glfwGetWindowUserPointer(window))->belt;
        //auto camera = LockedCamera::get_Active_Camera();
        glm::vec3 throwDirection = belt->throwTowardsCenter();
        if (throwDirection == glm::vec3(0.f)) {
            return;
        }
        //glm::vec3 intersectionOnUnitSphere = glm::normalize(camera->get_Camera_Position());
        planet->addCrater(throwDirection, belt->getThrowSpeed());
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        Planet *planet = ((WindowPointerParameters*) glfwGetWindowUserPointer(window))->planet;
        auto camera = Camera::get_Active_Camera();
        glm::vec3 throwDirection = -(camera->get_Camera_Front()); 
        if (throwDirection == glm::vec3(0.f)) {
            return;
        }
        //glm::vec3 intersectionOnUnitSphere = glm::normalize(camera->get_Camera_Position());
        planet->plant_trees(throwDirection,0.3f);
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        WindowPointerParameters *param = (WindowPointerParameters*) glfwGetWindowUserPointer(window);
        param->mousePosition = glm::vec2(xpos, ypos);
        param->mouseClicked = true;
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(glfwGetInputMode(window, GLFW_CURSOR) ==GLFW_CURSOR_DISABLED) {
    if (glfwRawMouseMotionSupported()) {;
        Camera *camera = Camera::get_Active_Camera();
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera->handle_mouse_motion_event(xoffset,yoffset);
    } else {
        std::cout << "hello" << std::endl;
    }
    }
}

//https://learnopengl.com/Getting-started/Hello-Window
void processInput(GLFWwindow *window, float deltaTime)
{

    auto camera = Camera::get_Active_Camera();
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_W, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_S, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_A, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera->handle_key_event(GLFW_KEY_D, deltaTime);
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
    float lastX =  (float)WIDTH/2;
    float lastY = (float)HEIGHT/2;
    glm::vec3 first_camera_Position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 first_camera_Target = glm::vec3(0.0,0.0,0.0);
    float speed = 1.0f;

    LockedCamera camera = LockedCamera(first_camera_Position,first_camera_Target,speed);
    //FreeFlightCamera camera2 = FreeFlightCamera(first_camera_Position,first_camera_Target,speed);
    //FreeFlightCamera camera3 = FreeFlightCamera(first_camera_Position,first_camera_Target,speed*2.0f);
    //camera2.set_As_Active_Camera();

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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

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

    Planet planet = Planet(seed);
    AsteroidBelt asteroidBelt = AsteroidBelt(seed);
    //hacky way to make object available in callback, TODO: change this
    WindowPointerParameters param = {&planet, &asteroidBelt, false, glm::vec2(0.f)};
    glfwSetWindowUserPointer(window, &param);
    Skybox skybox = Skybox();

    //string obj_file = (string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack/low_poly_tree_1.obj";
    //string material_directory = (string)Project_SOURCE_DIR + "/src/assets/LowpolyForestPack";

    //Model tree_model = Model(obj_file,material_directory);
    glm::vec3 planet_info = planet.getPlanetInfo(); // holds info about when to render certain biomes; TODO package into a struct called Planet_Config
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        current_Time = std::chrono::system_clock::now();
        elapsed_Time = current_Time - last_Time;
        deltaTime = (float)elapsed_Time.count();
        //std::cout << "Elapsed Time: "<< elapsed_Time.count() << endl;

        /*Input handling here*/
        processInput(window, (float)elapsed_Time.count());

        /*Update Game state*/
        //LightSource::getInstance().updatePosition();
        Camera::get_Active_Camera()->update_Camera_Shake();

        /* Render here */
        int current_width, current_height;
        glfwGetWindowSize(window, &current_width, &current_height);

        //Shadow Mapping
        LightSource::getInstance().prepareDepthMapCreation();

        planet.drawForDepthMap();
        asteroidBelt.drawForDepthMap();

        LightSource::getInstance().finishDepthMapCreation(current_width, current_height);

        //Main render passes
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilMask(0x00);

        if(param.mouseClicked) {
            asteroidBelt.pick(current_width, current_height, param.mousePosition);
            param.mouseClicked = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilMask(0x00);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        //pinetree_model.draw_instanced(current_width, current_height);
        planet.draw(current_width, current_height,planet_info);

        skybox.draw(current_width, current_height);// render skybox as last object in the scene, for optimization purposes.

        asteroidBelt.draw(current_width, current_height);

        gui_Object.imgui_Frame_Setup();
        gui_Object.imgui_Camera_Control_Window(&is_Locked_Camera,&is_Free_Camera,&current_Camera_Speed);
        gui_Object.imgui_Debug_Window(&is_Wireframe,planet_info);
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

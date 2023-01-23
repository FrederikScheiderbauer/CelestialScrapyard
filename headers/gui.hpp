#pragma once

#include "GLFW/glfw3.h"
#include "definitions.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Gui 
{
    public:
        void imgui_Init(GLFWwindow* window);
        void imgui_Frame_Setup();
        void imgui_Test_Window();  
        void imgui_Render();
        void imgui_Shutdown();
        void imgui_Camera_Control_Window(bool* is_Locked_Camera,bool* is_Free_Camera, float* current_speed);
        void imgui_Close_Game_Button();
        void imgui_FPS_Counter();
        void imgui_Debug_Window(bool* is_Wireframe, glm::vec3 &planet_info, float &pickedAsteroidTheta);
        void swap_to_LockedCamera();
        void swap_to_FreeFlightCamera();
};
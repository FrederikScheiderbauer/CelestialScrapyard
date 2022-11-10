#pragma once

#include "GLFW/glfw3.h"


class Gui {


    public:
        void imgui_Init(GLFWwindow* window);
        void imgui_Frame_Setup();
        void imgui_Test_Window();  
        void imgui_Render();
        void imgui_Shutdown();

};
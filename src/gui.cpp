#include "gui.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Gui::imgui_Frame_Setup() 
{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
}

void Gui::imgui_Init(GLFWwindow* window) 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 460");
}
void Gui::imgui_Debug_Window(bool* is_Wireframe)
{
    ImGui::Begin("Debug Window");
    if(ImGui::Checkbox("Wireframe Mode",is_Wireframe)) {}
    ImGui::End();
}
void Gui::imgui_Test_Window()
{
    //renders small text window element
    ImGui::Begin("New Window");
    ImGui::Text("Test");
    ImGui::End();
}
/*
Display Window that shows Options the Control of the Camera:
-Camera Speed -> Slider
-Camera Movement:
    -Locked Camera-> Camera that is always locked to look at Planet, but can move around it. Zoom in/out.
    -Freeflight Camera-> Camera with completely free controls
*/
void Gui::imgui_Camera_Control_Window(bool* is_Locked_Camera,bool* is_Free_Camera, float* current_Speed)
{
    ImGui::Begin("Camera Controls");
        if(ImGui::Checkbox("Locked Camera",is_Locked_Camera)) { *is_Free_Camera = !(*is_Locked_Camera);}
        if(ImGui::Checkbox("Freeflight Camera",is_Free_Camera)) {*is_Locked_Camera = !(*is_Free_Camera);}
        ImGui::SliderFloat("Camera Speed",current_Speed,0.0f,1.0f);
    ImGui::End();
}

void Gui::imgui_Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::imgui_Shutdown()
{
    //shutdown for ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
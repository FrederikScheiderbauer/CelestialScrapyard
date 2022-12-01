#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>

LockedCamera* LockedCamera::active_camera = nullptr;

//Camera Class Implementation






//





LockedCamera* LockedCamera::get_Active_Camera()
{
	return active_camera;
}

void LockedCamera::set_As_Active_Camera()
{
	active_camera = this;
}

LockedCamera::LockedCamera(glm::vec3 first_camera_Position,glm::vec3 first_camera_Target, float speed)
{

    cameraPos = first_camera_Position;
    cameraTarget = first_camera_Target;
    camera_Speed = speed;
    theta = std::acos((glm::normalize(first_camera_Position - first_camera_Target)).y);
    phi = std::atan((glm::normalize(first_camera_Position - first_camera_Target)).z /(glm::normalize(first_camera_Position - first_camera_Target)).x);
    active_camera = this;
    viewMatrix = glm::mat4(0.0f);
    update_Camera_State();
}

void LockedCamera::update_Camera_State()
{
    viewMatrix = glm::lookAt(cameraPos,cameraTarget,get_Camera_Up());
}

glm::vec3 LockedCamera::get_Camera_Position()
{
    return cameraPos;
}
glm::vec3 LockedCamera::get_Camera_Target()
{
    return cameraTarget;
}
glm::vec3 LockedCamera::get_Camera_Up()
{
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    return glm::cross(cameraDirection, cameraRight);
}
glm::vec3 LockedCamera::get_Up_Vektor()
{
    return up;
}
glm::vec3 LockedCamera::get_Camera_Front()
{
    return glm::normalize(cameraTarget - cameraPos);
}
float LockedCamera::get_Camera_Speed()
{
    return camera_Speed;
}
float LockedCamera::get_Theta()
{

    return theta;
}
float LockedCamera::get_Phi()
{
    return phi;
}


glm::mat4 LockedCamera::get_View_Matrix()
{
    return viewMatrix;
}



void LockedCamera::set_Camera_Position(glm::vec3 new_Camera_Position)
{
    cameraPos = new_Camera_Position;
}
void LockedCamera::set_Theta(float new_Theta)
{
    theta = new_Theta;
}
void LockedCamera::set_Phi(float new_Phi)
{
    phi = new_Phi;
}


bool LockedCamera::handle_key_event(int key)
{
    bool handled = false;
    if( key == GLFW_KEY_UP || key == GLFW_KEY_W)
    {
        theta -= camera_Speed * 0.1f;
        handled = true;
    }
    if( key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
    {
        theta += camera_Speed * 0.1f;
        handled = true;
    }
    if( key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
    {
        phi -= camera_Speed * 0.1f;
        handled = true;
    }
    if( key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
    {
        phi += camera_Speed * 0.1f;
        handled = true;
    }
    if ( !handled){return false;}
    update_Camera_Position();
    return handled;
}

bool LockedCamera::handle_scroll_event(float yoffset) {
        cameraPos += camera_Speed  * yoffset * glm::normalize(cameraTarget - cameraPos);
    return true;
}

void LockedCamera::update_Camera_Position(){
        float new_x_vec = glm::length(cameraPos - cameraTarget) * std::sin(theta) * std::cos(phi);
        float new_y_vec = glm::length(cameraPos - cameraTarget) * std::cos(theta);
        float new_z_vec = glm::length(cameraPos - cameraTarget) * std::sin(theta) * std::sin(phi);
        cameraPos = glm::vec3(new_x_vec,new_y_vec,new_z_vec);
}
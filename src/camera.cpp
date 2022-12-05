#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>

Camera* Camera::active_camera = nullptr;

//Camera Class Implementation

void Camera::update_Camera_State()
{
    viewMatrix = glm::lookAt(cameraPos,cameraTarget,get_Camera_Up());
}

glm::vec3 Camera::get_Camera_Position()
{
    return cameraPos;
}
glm::vec3 Camera::get_Camera_Target()
{
    return cameraTarget;
}
glm::vec3 Camera::get_Camera_Up()
{
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, camera_Front));
    return glm::cross(camera_Front, cameraRight);
}
glm::vec3 Camera::get_Up_Vektor()
{
    return up;
}
glm::vec3 Camera::get_Camera_Front()
{
    return camera_Front;
}
float Camera::get_Camera_Speed()
{
    return camera_Speed;
}

glm::mat4 Camera::get_View_Matrix()
{
    return viewMatrix;
}



void Camera::set_Camera_Position(glm::vec3 new_Camera_Position)
{
    cameraPos = new_Camera_Position;
}

Camera* Camera::get_Active_Camera()
{
	return active_camera;
}

void Camera::set_As_Active_Camera()
{
	active_camera = this;
}
//


LockedCamera::LockedCamera(glm::vec3 first_camera_Position,glm::vec3 first_camera_Target, float speed)
{

    cameraPos = first_camera_Position;
    cameraTarget = first_camera_Target;
    camera_Speed = speed;
    theta = std::acos((glm::normalize(first_camera_Position - first_camera_Target)).y);
    phi = std::atan((glm::normalize(first_camera_Position - first_camera_Target)).z /(glm::normalize(first_camera_Position - first_camera_Target)).x);
    active_camera = this;
    camera_Front = glm::normalize(cameraTarget - cameraPos);
    viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
}

void LockedCamera::update_Camera_State()
{
    float new_x_vec = glm::length(cameraPos - cameraTarget) * std::sin(theta) * std::cos(phi);
    float new_y_vec = glm::length(cameraPos - cameraTarget) * std::cos(theta);
    float new_z_vec = glm::length(cameraPos - cameraTarget) * std::sin(theta) * std::sin(phi);
    cameraPos = glm::vec3(new_x_vec,new_y_vec,new_z_vec);
    camera_Front = glm::normalize(cameraTarget - cameraPos);
    //make sure Camera stays within bounds, and doesnt flip around the poles
    if(theta < lowerBound){
        theta = lowerBound;
    }
    if( theta > upperBound){
        theta= upperBound;
    }
    viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
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
    update_Camera_State();
    return handled;
}

bool LockedCamera::handle_scroll_event(float xoffset, float yoffset) {
    cameraPos += camera_Speed  * yoffset * glm::normalize(cameraTarget - cameraPos);
    update_Camera_State();
    return true;
}



bool LockedCamera::handle_mouse_motion_event(float xoffset, float yoffset) {
    return false;
}
//FreeFlight Camera Implementation
/*
FreeFlightCamera::FreeFlightCamera(glm::vec3 _cameraPos,glm::vec3 _camera_Front,float _speed) {
    cameraPos = _cameraPos;
    camera_Front = _camera_Front;
    camera_Speed = _speed;
    viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
}

void FreeFlightCamera::update_Camera_State() {

}

bool FreeFlightCamera::handle_key_event(int key) {
    return false;
}

bool handle_scroll_event(float xoffset, float yoffset) {
    return false;
}
bool handle_mouse_motion_event(float xoffset, float yoffset) {
    return false;
}
*/
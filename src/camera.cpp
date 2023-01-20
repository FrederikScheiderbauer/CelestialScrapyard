#include "../headers/camera.hpp"
#include "../headers/Random.hpp"
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

void Camera::start_Camera_Shake() {
    cameraShakeAmplitude = 0.05f;
    lastFrameOffset = glm::vec3(0.f);
}

void Camera::update_Camera_Shake() {
    if (cameraShakeAmplitude < 1E-5) {
        return;
    }
    glm::vec3 offset;
    offset.x = Random::getInRange(-1.f, 1.f);
    offset.y = Random::getInRange(-1.f, 1.f);
    offset.z = Random::getInRange(-1.f, 1.f);
    offset *= cameraShakeAmplitude;
    cameraPos = cameraPos - lastFrameOffset + offset;
    lastFrameOffset = offset;
    viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
    cameraShakeAmplitude -= CAMERA_SHAKE_DECREMENT;
    if (cameraShakeAmplitude < 1E-5) {
        cameraPos -= lastFrameOffset;
    }
}


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



bool LockedCamera::handle_key_event(int key, float deltaTime)
{
    bool handled = false;
    if( key == GLFW_KEY_UP || key == GLFW_KEY_W)
    {
        theta -= camera_Speed * deltaTime;
        handled = true;
    }
    if( key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
    {
        theta += camera_Speed * deltaTime;
        handled = true;
    }
    if( key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
    {
        phi -= camera_Speed * deltaTime;
        handled = true;
    }
    if( key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
    {
        phi += camera_Speed * deltaTime;
        handled = true;
    }
    if ( !handled){return false;}
    update_Camera_State();
    return handled;
}

bool LockedCamera::handle_scroll_event(float xoffset, float yoffset, float deltaTime) {
    cameraPos += deltaTime * 10.f * camera_Speed  * yoffset * glm::normalize(cameraTarget - cameraPos);
    update_Camera_State();
    return true;
}



bool LockedCamera::handle_mouse_motion_event(float xoffset, float yoffset) {
    return false;
}
//FreeFlight Camera Implementation

FreeFlightCamera::FreeFlightCamera(glm::vec3 first_camera_Position,glm::vec3 first_camera_Target, float speed)
{

    cameraPos = first_camera_Position;
    cameraTarget = first_camera_Target;
    camera_Speed = speed;
    active_camera = this;
    pitch = -90.0f;
    yaw = 0.0f;
    camera_Front = glm::normalize(cameraTarget - cameraPos);
    viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
}

void FreeFlightCamera::update_Camera_State()
{
viewMatrix = glm::lookAt(cameraPos,cameraPos + camera_Front,get_Camera_Up());
}



bool FreeFlightCamera::handle_key_event(int key, float deltaTime)
{
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, camera_Front));
    bool handled = false;
    if( key == GLFW_KEY_UP || key == GLFW_KEY_W)
    {
        cameraPos += camera_Speed  * deltaTime * camera_Front;
        handled = true;
    }
    if( key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
    {
        cameraPos -= camera_Speed  *deltaTime * camera_Front;
        handled = true;
    }
    if( key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
    {
        cameraPos -= camera_Speed  * deltaTime * cameraRight;
        handled = true;
    }
    if( key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
    {
        cameraPos += camera_Speed  * deltaTime * cameraRight;
        handled = true;
    }
    if ( !handled){return false;}
    update_Camera_State();
    return handled;
}

bool FreeFlightCamera::handle_scroll_event(float xoffset, float yoffset, float deltaTime) {

    return false;
}



bool FreeFlightCamera::handle_mouse_motion_event(float xoffset, float yoffset) {
   
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_Front = glm::normalize(direction);
    update_Camera_State();
    return true;
}
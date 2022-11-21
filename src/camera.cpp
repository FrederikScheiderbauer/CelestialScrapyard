#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

Camera* Camera::active_camera = nullptr;

Camera* Camera::get_Active_Camera()
{
	return active_camera;
}

void Camera::set_As_Active_Camera()
{
	active_camera = this;
}

Camera::Camera(glm::vec3 first_camera_Position,glm::vec3 first_camera_Target, float speed)
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
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    return glm::cross(cameraDirection, cameraRight);
}
glm::vec3 Camera::get_Up_Vektor()
{
    return up;
}
glm::vec3 Camera::get_Camera_Front()
{
    return glm::normalize(cameraTarget - cameraPos);
}
float Camera::get_Camera_Speed()
{
    return camera_Speed;
}
float Camera::get_Theta()
{

    return theta;
}
float Camera::get_Phi()
{
    return phi;
}


glm::mat4 Camera::get_View_Matrix()
{
    return viewMatrix;
}



void Camera::set_Camera_Position(glm::vec3 new_Camera_Position)
{
    cameraPos = new_Camera_Position;
}
void Camera::set_Theta(float new_Theta)
{
    theta = new_Theta;
}
void Camera::set_Phi(float new_Phi)
{
    phi = new_Phi;
}
#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(glm::vec3 first_camera_Position,glm::vec3 first_camera_Target,glm::vec3 first_up, float speed)
{
    cameraPos = first_camera_Position;
    cameraTarget = first_camera_Target;
    up = first_up;
    camera_Speed = speed;
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

/*
glm::vec3 Camera::get_View_Matrix()
{

}
*/


void Camera::set_Camera_Position(glm::vec3 new_Camera_Position)
{
    cameraPos = new_Camera_Position;
}
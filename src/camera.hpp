#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
    private:
        void update_Camera_State();
        static Camera* active_camera;
        glm::vec3 cameraPos;
        glm::vec3 cameraTarget;
        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
        glm::mat4 viewMatrix;
        float camera_Speed;
        float theta;
        float phi;

    public:
        Camera(glm::vec3 cameraPos,glm::vec3 cameraTarget,float speed); 
        glm::vec3 get_Camera_Position();
        float get_Camera_Speed();
        float get_Theta();
        float get_Phi();
        glm::vec3 get_Camera_Front();
        glm::vec3 get_Camera_Target();
        glm::vec3 get_Camera_Up();
        glm::vec3 get_Up_Vektor();
        glm::mat4 get_View_Matrix();
        static Camera* get_Active_Camera();
        void set_Camera_Position(glm::vec3 cameraPos);
        void set_Camera_Speed(glm::vec3 cameraPos);
        void set_Camera_Target(glm::vec3 cameraTarget);
        void set_Up_Vektor(glm::vec3 up);
        void set_Theta(float new_Theta);
        void set_Phi(float new_Phi);
        void set_As_Active_Camera();
        bool handle_key_event(int key);
};

class LockedCamera : public Camera
{
    private:
        float l_theta;
        float l_phi;
        float radius;


    public:
        bool handle_Keyboard_Event(int key);
        bool handle_Mouse_Scroll_Event(int key);
};
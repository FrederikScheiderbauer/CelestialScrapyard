#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {
    public:
        Camera() {};
        virtual bool handle_key_event(int key) = 0;
        virtual bool handle_scroll_event(float yoffset) = 0;

    protected:
        glm::vec3 cameraPos;
        glm::vec3 cameraTarget;
        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
        glm::mat4 viewMatrix;
        float camera_Speed;
};

class LockedCamera : public Camera
{
    private:
        void update_Camera_State();
        static LockedCamera* active_camera;
        float theta;
        float phi;

    public:
        LockedCamera(glm::vec3 cameraPos,glm::vec3 cameraTarget,float speed); 
        glm::vec3 get_Camera_Position();
        float get_Camera_Speed();
        float get_Theta();
        float get_Phi();
        glm::vec3 get_Camera_Front();
        glm::vec3 get_Camera_Target();
        glm::vec3 get_Camera_Up();
        glm::vec3 get_Up_Vektor();
        glm::mat4 get_View_Matrix();
        static LockedCamera* get_Active_Camera();
        void set_Camera_Position(glm::vec3 cameraPos);
        void set_Camera_Speed(glm::vec3 cameraPos);
        void set_Camera_Target(glm::vec3 cameraTarget);
        void set_Up_Vektor(glm::vec3 up);
        void set_Theta(float new_Theta);
        void set_Phi(float new_Phi);
        void set_As_Active_Camera();

        bool handle_key_event(int key) override;
        bool handle_scroll_event(float yoffset) override;
        void update_Camera_Position();
};

/*
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
*/
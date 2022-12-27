#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
        Camera() {};
        virtual bool handle_key_event(int key, float deltaTime) = 0;
        virtual bool handle_mouse_motion_event(float xoffset, float yoffset) = 0;
        virtual bool handle_scroll_event(float xoffset, float yoffset) = 0;
        glm::mat4 get_View_Matrix();
        void set_Camera_Position(glm::vec3 new_Camera_Position);
        glm::vec3 get_Camera_Position();
        glm::vec3 get_Camera_Target();
        glm::vec3 get_Camera_Up();
        glm::vec3 get_Up_Vektor();
        glm::vec3 get_Camera_Front();
        float get_Camera_Speed();
        static Camera* get_Active_Camera();
        void set_As_Active_Camera();

    protected:
        glm::vec3 cameraPos;
        glm::vec3 cameraTarget;
        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 camera_Front;
        glm::mat4 viewMatrix = glm::mat4(0.0f);
        float camera_Speed;
        static Camera* active_camera;
        virtual void update_Camera_State() = 0;
};

class LockedCamera : public Camera
{
    private:
        void update_Camera_State() override;
        float theta;
        float phi;
        float upperBound = 3.1f;
        float lowerBound = 0.15f;;

    public:
        LockedCamera(glm::vec3 cameraPos,glm::vec3 cameraTarget,float speed); 
        bool handle_mouse_motion_event(float xoffset, float yoffset) override;
        bool handle_key_event(int key,float deltaTime) override;
        bool handle_scroll_event(float xoffset, float yoffset) override;
};

class FreeFlightCamera : public Camera {
    private:
        void update_Camera_State() override;
        float pitch;
        float yaw;
        
    public:
        FreeFlightCamera(glm::vec3 cameraPos,glm::vec3 camera_Front,float speed);
        bool handle_mouse_motion_event(float xoffset, float yoffset) override;
        bool handle_key_event(int key, float deltaTime) override;
        bool handle_scroll_event(float xoffset, float yoffset) override;
};

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera {
    public:
        Camera() {};
        virtual bool handle_key_event(int key) = 0;
        virtual bool handle_scroll_event(float yoffset) = 0;
        glm::mat4 get_View_Matrix();
        void set_Camera_Position(glm::vec3 new_Camera_Position);
        void update_Camera_State();
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
        glm::mat4 viewMatrix;
        float camera_Speed;
        static Camera* active_camera;
};

class LockedCamera : public Camera
{
    private:
        void update_Camera_Position();
        void update_Camera_State();
        float theta;
        float phi;
        float upperBound = 3.1f;
        float lowerBound = 0.15f;;

    public:
        LockedCamera(glm::vec3 cameraPos,glm::vec3 cameraTarget,float speed); 
        float get_Theta();
        float get_Phi();

        void set_Theta(float new_Theta);
        void set_Phi(float new_Phi);

        bool handle_key_event(int key) override;
        bool handle_scroll_event(float yoffset) override;
};

class FreeFlightCamera : public Camera {
    private:

    public:
        FreeFlightCamera(glm::vec3 cameraPos,glm::vec3 cameraTarget,float speed);

        bool handle_key_event(int key) override;
        bool handle_scroll_event(float yoffset) override;
};
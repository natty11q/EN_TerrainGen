#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "ShaderClass.hpp"


class Camera
{
    public:
        // Stores the main vectors of the camera
        glm::vec3 Position;
        glm::vec3 StartPos;
        glm::vec3 Orientation;
        glm::vec3 Up;
        glm::mat4 cameraMatrix;

        // Prevents the camera from jumping around when first clicking left click
        bool firstClick;

        // Stores the width and height of the window
        int width;
        int height;

        // Adjust the speed of the camera and it's sensitivity when looking around
        float speed;
        float sensitivity;

        // Camera constructor to set up initial values
        Camera(int width, int height, glm::vec3 position, float FOVdeg, float nearPlane, float farPlane);

        void Setup();

        // Updates and exports the camera matrix to the Vertex Shader
        void updateMatrix();
        void Matrix(ShaderClass& shader, const char* uniform);
        // Handles camera inputs
        void Inputs(GLFWwindow* window,float deltatime);



        float AspectRatio;
    private:

        float FOVdeg;
        float nearPlane;
        float farPlane;
        float TopSpeed;
        float BaseSpeed;
        bool MouseIn;
};


#endif // !CAMERA_HPP

#include "Camera.hpp"
#include "GLFW/glfw3.h"
#include <cstdlib>

Camera::Camera(int width, int height, glm::vec3 position , float FOVdeg, float nearPlane, float farPlane)
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
	Camera::StartPos = position;
	Camera::FOVdeg = FOVdeg;
	Camera::farPlane = farPlane;
	Camera::nearPlane = nearPlane;
	Camera::MouseIn = false;

    Setup();
}


void Camera::Setup()
{
    Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    firstClick = true;

    TopSpeed   = 10.0f;
    BaseSpeed  = 1.0f;

    speed = BaseSpeed;

    sensitivity = 100.0f;

    cameraMatrix = glm::mat4(1.0f);

}

void Camera::updateMatrix()
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	this->AspectRatio = ((float)width / (float)height);
	projection = glm::perspective(glm::radians(FOVdeg), AspectRatio, nearPlane, farPlane);

    cameraMatrix = projection * view;

}

void Camera::Matrix(ShaderClass& shader, const char* uniform)
{
	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.Get_id(), uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

}



void Camera::Inputs(GLFWwindow* window , float deltatime)
{
	glfwGetWindowSize(window, &this->width,&this->height);
	// // Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		Position = StartPos;
	}

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		this->farPlane += 100.0f * deltatime * 1000;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		this->farPlane -= 100.0f * deltatime * 1000;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		this->nearPlane += 10.0f * deltatime * 1000;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		this->nearPlane -= 10.0f * deltatime * 1000;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		this->FOVdeg += 5.0f * deltatime * 1000;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		this->FOVdeg -= 5.0f * deltatime * 1000;
	}



	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	// if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	// {
	// 	Position += speed * -Orientation;
	// }
	// if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	// {
	// 	Position += speed * glm::normalize(glm::cross(Orientation, Up));
	// }
	// if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	// {
	// 	Position += speed * Up;
	// }
	// if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	// {
	// 	Position += speed * -Up;
	// }
	// if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	// {
	// 	speed = 0.4f;
	// }
	// else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	// {
	// 	speed = 0.1f;
	// }
	// Handles mouse inputs

	if(glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS)
	{
		MouseIn = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !MouseIn)
	{
		MouseIn = true;
	}




	if (MouseIn)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		// glfwSetCursorPos(window, (width / 2), (height / 2));
		// firstClick = false;

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));



	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	} 
}

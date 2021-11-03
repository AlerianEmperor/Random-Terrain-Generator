#ifndef _CONTROLS_
#define _CONTROLS_

#include <gl\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Diamond_Square.h"

unsigned int screenWidth = 1024;
unsigned int screenHeight = 768;

using namespace std;
using namespace glm;

struct Camera
{
	vec3 p;
	vec3 direction;
	float HorizontalAngle;
	float VerticalAngle;
	float mouseSpeed;
	float moveSpeed;
	float fov;
	bool generate = false;
	bool Save = false;
	mat4 pMat;
	mat4 vMat;
	mat4 mMat;
	
	Camera()
	{
		//p = vec3(2.0f, 24.0f, 1.0f);//2 14, 1
		//direction = vec3(2, 14, 1.0f);// 2, 14, 1

		//p = vec3(2.0f, 0.0f, 1.0f);//2 14, 1
		//direction = vec3(2, 14, 1.0f);// 2, 14, 1

		//p = vec3(328.307739, 1087.504833, 1290.489136);
		//direction = vec3(0.752683, -0.470626, 0.460413);

		//p = vec3(328.3077, 1087.5048, 1290.4891);
		//direction = vec3(0.8017, -0.4077, 0.4369);

		//p = vec3(-1102.7348, 2536.7973, 638.4397);
		//direction = vec3(0.790583, -0.4077, 0.4568);

		p = vec3(4486.6562, -1368.1281, 1984.8083);
		//direction = vec3(-0.9118, 0.504, 0.47705);

		direction = vec3(-0.6118, 0.7904, 0.02705);
		
		
		//direction = vec3(-0.6118, -0.2904, 0.02705);

		HorizontalAngle = 4.71f;
		VerticalAngle = 0.0f;
		mouseSpeed = 0.005f;
		moveSpeed = 18.0f;
		fov = 45.0f;
		mMat = mat4(1.0f);
	}
	Camera(vec3 pos, float HorizontalAngle_, float VerticalAngle_, float mouseSpeed_, float moveSpeed_, float fov_) :
		p(pos), HorizontalAngle(HorizontalAngle_), VerticalAngle(VerticalAngle_), mouseSpeed(mouseSpeed_), moveSpeed(moveSpeed_), fov(fov_)
	{}

	void Compute_Matrix(GLFWwindow*& window)
	{
		double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();

		float deltaTime = currentTime - lastTime;

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		glfwSetCursorPos(window, screenWidth / 2, screenHeight / 2);

		HorizontalAngle -= mouseSpeed * float(screenWidth / 2 - xPos);
		VerticalAngle += mouseSpeed * float(screenHeight / 2 - yPos);

		direction = vec3(
			cosf(VerticalAngle) * sinf(HorizontalAngle),
			sinf(VerticalAngle),
			cosf(VerticalAngle) * cosf(HorizontalAngle));

		vec3 right(
			sinf(HorizontalAngle - 3.1415926 / 2.0f),
			0,
			cosf(HorizontalAngle - 3.1415926 / 2.0f));

		vec3 up(cross(right, direction));
		generate = false;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			p += direction * moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			p -= direction * moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			p += right * moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			p -= right * moveSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			generate = true;
		}
		/*if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)//S is already used
		{
			Save = true;
		}*/
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwTerminate();
		}

		pMat = perspective(fov, float(screenWidth / screenHeight), 0.1f, 90000.0f);
		vMat = lookAt(p, p + direction, up);

		lastTime = currentTime;
	}

};

#endif // !_CONTROLS_

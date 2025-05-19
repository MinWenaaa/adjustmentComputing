#include "windowPara.h"
#include <iostream>

WindowParas::WindowParas() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GL_FALSE);

	this->window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL_Learning", NULL, NULL);

	if (window == NULL) {
		std::cout << "Wrong!" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Wrong!" << std::endl;
	}
	defaultAlpha = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	std::cout << glGetError() << std::endl;
}

void WindowParas::updateScreenSize(GLint width, GLint height) {
	SCREEN_WIDTH = width; SCREEN_HEIGHT = height;
	defaultAlpha = (float)width / height;
}
// ------------------------------------------------------------------------
GLfloat WindowParas::screen2normalX(GLdouble screenX) const {
	//return (screenX - SCREEN_WIDTH / 2) / displayWidth / 2;
	return  (2.0f * static_cast<GLfloat>(screenX / SCREEN_WIDTH * xScale)) - 1.0f;
}
GLfloat WindowParas::screen2normalY(GLdouble screenY) const {
	//return (screenY - SCREEN_HEIGHT / 2) / displayHeight / 2;
	return 1.0f - (2.0f * static_cast<GLfloat>(screenY / SCREEN_HEIGHT * yScale));
}
GLfloat WindowParas::normal2orthoX(GLfloat normalX) const {
	GLfloat left = -SCREEN_WIDTH / xScale / 2.0f;
	GLfloat right = SCREEN_WIDTH / xScale / 2.0f;
	return  left + (right - left) * (normalX + 1) / 2;
}
GLfloat WindowParas::normal2orthoY(GLfloat normalY) const {
	GLfloat button = -SCREEN_HEIGHT / yScale / 2.0f;
	GLfloat top = SCREEN_HEIGHT / yScale / 2.0f;
	return  button + (top - button) * (normalY + 1) / 2;
}
// ------------------------------------------------------------------------

void Camera::get2Dxy(float oX, float oY, float& rX, float& rY) {
	//std::cout << "angle correct" << atan(oX / f) << ", " << atan(oY / f) << std::endl;
	float newElevation = elevationAngle - atan(oY / f);
	float plantDistance = cameraPos.y / tan(newElevation);
	rX = plantDistance * sin(azimuthAngle) - cameraPos.x;
	rY = plantDistance * cos(azimuthAngle) - cameraPos.z;
	//std::cout << "delta:" << newElevation << std::endl;
	//float plantDistance;
	//if (newElevation > 0) plantDistance = distance * sin(newElevation) / sin(elevationAngle-newElevation);
	//else plantDistance = distance * sin(-newElevation) / sin(elevationAngle);
	//rX = sin(azimuthAngle) * plantDistance;
	//rY = -cos(azimuthAngle) * plantDistance;
	float deltaHorizon = oX / f * cameraPos.y / sin(newElevation);

	//std::cout << "distance: " << plantDistance << ", " << deltaHorizon << std::endl;
	rX += cos(azimuthAngle) * deltaHorizon;
	rY += sin(azimuthAngle) * deltaHorizon;

	//rX = rX*sin(azimuthAngle)
	//std::cout << "plant Distamce " << rX*rX + rY*rY << std::endl;
}
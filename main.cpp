#include<iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "solution.h"
#include "windowPara.h"
#include "MyGUI.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool isDragging;
double lastX, lastY;
GLfloat press_time;

int main()
{
    GLFWwindow* window = WindowParas::instance().window;
    WindowParas::instance().setRatio(1);

    // 回调绑定
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

	MyGUI::getInstance().init(window);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		solution::instance().Render();
		MyGUI::getInstance().Render(600, 1000);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
	WindowParas::instance().updateScreenSize(w, h);
	solution::instance().adaptation((float)w / h);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {	// 缩放
	Camera::getInstance().changeZoom(yoffset * 0.1);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	WindowParas& windowPara = WindowParas::instance();
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	lastX = windowPara.screen2normalX(x);
	lastY = windowPara.screen2normalY(y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		press_time = glfwGetTime();
		isDragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isDragging = false;
		
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	WindowParas& windowPara = WindowParas::instance();
	xpos = windowPara.screen2normalX(xpos);
	ypos = windowPara.screen2normalY(ypos);
	if (isDragging) {
		Camera::getInstance().changeElevation(lastY - ypos);
		Camera::getInstance().chopdeltaX(lastX - xpos);
		lastX = xpos;
		lastY = ypos;
	}
}
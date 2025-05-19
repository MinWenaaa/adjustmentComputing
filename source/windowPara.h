#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float FOV = glm::radians(60.f);
const float f = 1 / tan(FOV / 2);

class WindowParas {
public:
	static WindowParas& instance() {
		static WindowParas instance;
		return instance;
	}
	GLFWwindow* window;

	void setRatio(GLfloat ratio) {
		aspectRatio = ratio;
		updateScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	void updateScreenSize(GLint width, GLint height);

	GLint SCREEN_WIDTH = 1600, SCREEN_HEIGHT = 1200;
	GLfloat aspectRatio = 1;
	GLint displayWidth, displayHeight;
	GLfloat xScale = 1, yScale = 1;
	GLfloat defaultAlpha;

	GLfloat screen2normalX(GLdouble screenX) const;
	GLfloat screen2normalY(GLdouble screenY) const;
	GLfloat normal2orthoX(GLfloat normalX) const;
	GLfloat normal2orthoY(GLfloat normalY) const;

private:
	WindowParas();
	WindowParas(const WindowParas&) = delete;
	WindowParas& operator=(const WindowParas&) = delete;

};

const double PI = 3.1415926;
class Camera {
public:
	static Camera& getInstance() {
		static Camera instance;
		return instance;
	}
	Camera(const Camera&) = delete;
	void operator=(const Camera&) = delete;

	glm::mat4 getView() {
		return view;
	}
	void changeMoving() { mapMoving = !mapMoving; }
	bool getMoving() { return mapMoving; }

	void changeZoom(float zoom) { distance -= zoom; if (distance < 0.5f) distance = 0.5f; upDataView(); }
	void chopdeltaX(float delta) {
		if (!mapMoving) {
			azimuthAngle += delta;
		}
		else {
			offset.x += delta / 2;
			cameraTarget.x += delta / 2;
		}
		upDataView();
	}
	void changeElevation(float delta) {
		if (mapMoving) {
			offset.y += delta / 2;
			cameraTarget.y += delta / 2;
		}
		else {
			elevationAngle += delta;
			if (elevationAngle > (PI / 2)) elevationAngle = PI / 2;
			if (elevationAngle < 0.4f) elevationAngle = 0.4f;
		}
		upDataView();
	}

	void get2Dxy(float oX, float oy, float& rX, float& rY);


private:
	Camera() : distance(1.5f), elevationAngle(0.7f), azimuthAngle(-0.3f), cameraTarget(glm::vec3(0.f, 0.f, 0.f)) {
		upDataView();
	}

	void upDataView() {
		GLfloat plant = distance * cos(elevationAngle);
		cameraPos = glm::vec3(plant * sin(azimuthAngle), -plant * cos(azimuthAngle), distance * sin(elevationAngle)) + offset;
		view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	GLfloat distance, elevationAngle, azimuthAngle;
	glm::vec3 cameraPos, offset;
	glm::vec3 cameraTarget;
	glm::mat4 view;

	bool mapMoving = false;

};
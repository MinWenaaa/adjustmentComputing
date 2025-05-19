#pragma once
#include <string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

class Point {
public:
	Point(const std::string& name, double x, double y, double z, bool type);

	void draw();
	void changeStatus(bool status) {
		isSelected = status;
	}

	std::string name;

private:
	bool type; //0: controal point, 1: path point
	bool isSelected = false;
	GLfloat x, y, z;

	GLuint VAO, VBO_p, VBO_c;
};

class edge {
public:
	Point* start, * end;

	edge(Point* start, Point* end);

	void draw();
	void changeStatus(bool status) {
		isSelected = status;
	}

private:
	bool isSelected = false;
	GLuint VAO, VBO_p, VBO_c;
};
#pragma once
#include <vector>
#include <map>
#include "geo_object.h"
#include "shader.h"

class solution {
	friend class MyGUI;
public:
	static solution& instance() {
		static solution instance;
		return instance;
	}

	bool readFile(const char* filename);
	void ProcessInput(GLfloat x, GLfloat y);
	void Render();

	void adaptation(float a);

	std::vector<Point*> points;
	std::vector<edge*> edges;
	std::vector<int> closed_road[4];
	std::string inf[4];
	std::string netinfo;

	Shader* defaultShader;

	glm::mat4 projection;

	void change_focus(int i);
	void focus_point(int i);
private:
	solution();
	solution(const solution&) = delete;
	solution& operator=(const solution&) = delete;

	std::map<std::string, int> name2num;
};

#pragma once
#include <vector>
#include <map>
#include "geo_object.h"
#include "shader.h"

class solution {
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

	Shader* defaultShader;

	glm::mat4 projection;

private:
	solution();
	solution(const solution&) = delete;
	solution& operator=(const solution&) = delete;

	std::map<std::string, int> name2num;
};

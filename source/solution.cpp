#include<fstream>
#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "solution.h"
#include "WindowPara.h"

const double std_x = 3371100;
const double std_y = 558600;
const double ratio = 400;

solution::solution() : defaultShader(new Shader("source/shader/point.vs", "source/shader/point.fs")) {

	readFile("source/data.txt");

	projection = glm::mat4(1.0f);
	projection = glm::perspective(FOV, WindowParas::instance().defaultAlpha, 0.1f, 100.0f);
	defaultShader->setMat4("projection", projection);
}

bool solution::readFile(const char* filename) {
	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return false;
	}

	std::string line;
	std::getline(infile, line);
	if (line != "POINT_DATA:") return false;
	std::getline(infile, line);
	if (line != "point_num:") return false;

	int point_num;
	infile >> point_num;
	points.resize(point_num);
	std::string name;
	double x, y, z;
	bool type;
	for (int i = 0; i < point_num; i++) {
		infile >> name >> x >> y >> z >> type;
		x = (x - std_x) / ratio; y = (y - std_y) / ratio; z = z / ratio;
		points[i] = new Point(name, x, y, z, type);
		name2num[name] = i;
	}

	//std::getline(infile, line);
	//if (line != "CLOSE_ROAD_DATA:") return false;
	//std::getline(infile, line);
	//if (line != "road_num:") return false;
	//int road_num;	
	//infile >> road_num;
	//edges.resize(road_num);
	//std::string start, end;
	//infile >> start;
	//for (int i = 0; i < road_num; i++) {
	//	infile >> end;
	//	edges[i] = new edge(points[name2num[start]], points[name2num[end]]);
	//	start = end;
	//}

}

void solution::Render() {
	defaultShader->setMat4("view", Camera::getInstance().getView());
	defaultShader->use();
	for (int i = 0; i < points.size(); i++) {
		points[i]->draw();
	}
}
#include<fstream>
#include<iostream>
#include<sstream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "solution.h"
#include "WindowPara.h"

const double std_x = 3371300;
const double std_y = 558900;
const double std_z = 20;
const double ratio = 200;

solution::solution() : defaultShader(new Shader("source/shader/point.vs", "source/shader/point.fs")) {

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
		x = (x - std_x) / ratio; y = (y - std_y) / ratio; z = (z - std_z) * 4 / ratio;
		points[i] = new Point(name, x, y, z, type);
		name2num[name] = i;
	}

	std::getline(infile, line);
	std::getline(infile, line);
	if (line != "EDGE_DATA:") return false;
	std::getline(infile, line);
	if (line != "edge_num:") return false;
	int edge_num;	
	infile >> edge_num;
	edges.resize(edge_num);
	std::string start, end;
	for (int i = 0; i < edge_num; i++) {
		infile >> start >> end;
		edges[i] = new edge(points[name2num[start]], points[name2num[end]]);
	}

	std::getline(infile, line);
	std::getline(infile, line);
	if (line != "CLOSE_ROAD_DATA:") return false;
	std::getline(infile, line);
	if (line != "road_num:") return false;
	int road_num;
	infile >> road_num;
	for (int i = 0; i < road_num; ++i) {
		std::string line;
		std::getline(infile, line);
		while (line.empty() && infile) std::getline(infile, line);

		std::istringstream iss(line);
		int len;
		iss >> len;
		for (int j = 0; j < len; ++j) {
			std::string station;
			iss >> station;
			closed_road[i].push_back(name2num[station]);
		}

		std::string info1, info2;
		std::getline(infile, info1);
		std::getline(infile, info2);
		std::string info = info1 + "\n" + info2;
		inf[i] = info;
	}

	std::getline(infile, line);
	if (line != "NET_DATA:")return false;
	std::string restOfFile((std::istreambuf_iterator<char>(infile)),
		std::istreambuf_iterator<char>());
	netinfo = restOfFile;

}

void solution::Render() {
	defaultShader->setMat4("view", Camera::getInstance().getView());
	defaultShader->use();
	for (int i = 0; i < points.size(); i++) {
		points[i]->draw();
	}
	for (int i = 0; i < edges.size(); i++) {
		edges[i]->draw();
	}
}

void solution::adaptation(float a) {
	projection = glm::perspective(FOV, a, 0.1f, 100.0f);
	defaultShader->setMat4("projection", projection);
}

void solution::change_focus(int i) {
	for (auto p : points) {
		p->changeStatus(false);
	}
	for (auto e : edges) {
		e->changeStatus(false);
	}

	for (int idx : closed_road[i]) {
		points[idx]->changeStatus(true);
	}

	// 选中closed_road[i]中相邻点之间的边
	for (size_t j = 0; j + 1 < closed_road[i].size(); ++j) {
		int a = closed_road[i][j];
		int b = closed_road[i][j + 1];
		for (auto e : edges) {
			if ((e->start == points[a] && e->end == points[b]) ||
				(e->start == points[b] && e->end == points[a])) {
				e->changeStatus(true);
			}
		}
	}
}

void solution::focus_point(int i) {
	for (auto p : points) {
		p->changeStatus(false);
	}
	for (auto e : edges) {
		e->changeStatus(false);
	}
	points[i]->changeStatus(true);
}

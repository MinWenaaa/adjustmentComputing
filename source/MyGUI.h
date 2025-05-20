#pragma once

#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>
#include <string>

class MyGUI {
	friend class solution;
public:
	static MyGUI& getInstance() {
		static MyGUI instance;
		return instance;
	}
	MyGUI(const MyGUI&) = delete;
	MyGUI& operator=(const MyGUI&) = delete;

	void init(GLFWwindow* window);
	void Render(int w, int h);
	std::string point_name;

private:
	MyGUI() {}

	bool isShowDrag = false;
	std::string text = "";
	float fValue = 0.5f;

	bool inited = false;

	const char* getData(int i);

	int temp_status=0;
};

#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Shader {
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, float f1, float f2);
	void setVec3(const std::string& name, float f1, float f2, float f3);
	void setVec4(const std::string& name, float f1, float f2, float f3, float f4);
	void setMat4(const std::string& name, const glm::mat4& mat);
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};
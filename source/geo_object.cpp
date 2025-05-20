#include <cmath>
#include <vector>

#include "geo_object.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float r = 0.008;
const float r2 = 0.018;
const float r_edge = 0.002;

Point::Point(const std::string& name, double x, double y, double z, bool type):
    name(name), x(x), y(y), z(z), type(type) {
    if(type) {
        GLfloat tetrahedronVertices[12] = {
            x, y + r2, z- r2,
            x - r2, y, z + r2 ,
            x + r2, y, z + r2 ,
            x, y - r2 , z - r2
        };
        GLuint indices[] = {
			0, 1, 2, 0, 1, 3, 1, 2, 3, 0, 2, 3,
        };

        GLfloat tetrahedronColors[4] = { 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_p);
        glGenBuffers(1, &VBO_c);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // 顶点坐标
        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tetrahedronVertices), tetrahedronVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // 颜色
        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tetrahedronColors), tetrahedronColors, GL_STATIC_DRAW);
        glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
        glEnableVertexAttribArray(1);

        // 索引
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        this->indexCount = 12; // 4个三角面*3
    }
    else {
        GLfloat cubeVertices[] = {
            x - r, y - r, z + r, // 前面
            x + r, y - r, z + r,
            x + r, y + r, z + r,
            x - r, y + r, z + r,
            x - r, y - r, z - r, // 后面
            x + r, y - r, z - r,
            x + r, y + r, z - r,
            x - r, y + r, z - r
        };
        
        GLuint indices[] = {  // 36 个顶点索引（三角形绘制）
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4
        };

        GLfloat cubeColors[8] = { 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_p);
        glGenBuffers(1, &VBO_c);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // 绑定坐标数据到 VBO_p
        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // 绑定颜色数据到 VBO_c
        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
        glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        this->indexCount = 36;
    }
}

void Point::draw() {
    glBindVertexArray(VAO);
    if (type) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

edge::edge(Point* start, Point* end) {
    this->start = start;
    this->end = end;

    // 1. 计算主轴方向
    glm::vec3 p0(start->x, start->y, start->z);
    glm::vec3 p1(end->x, end->y, end->z);
    glm::vec3 axis = glm::normalize(p1 - p0);

    // 2. 找到与主轴不平行的向量，构造正交基
    glm::vec3 up = glm::abs(axis.z) < 0.99f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    glm::vec3 v1 = glm::normalize(glm::cross(axis, up));
    glm::vec3 v2 = glm::normalize(glm::cross(axis, v1));

    // 3. 计算起点和终点的三角形顶点
    std::vector<glm::vec3> baseA, baseB;
    for (int i = 0; i < 3; ++i) {
        float angle = glm::two_pi<float>() * i / 3.0f;
        glm::vec3 offset = r_edge * (cos(angle) * v1 + sin(angle) * v2);
        baseA.push_back(p0 + offset);
        baseB.push_back(p1 + offset);
    }
    // 合并为顶点数组
    std::vector<GLfloat> vertices;
    for (int i = 0; i < 3; ++i) {
        vertices.push_back(baseA[i].x); vertices.push_back(baseA[i].y); vertices.push_back(baseA[i].z);
    }
    for (int i = 0; i < 3; ++i) {
        vertices.push_back(baseB[i].x); vertices.push_back(baseB[i].y); vertices.push_back(baseB[i].z);
    }

    std::vector<GLuint> indices = {
        // 起点三角面
        0, 1, 2,
        // 终点三角面
        3, 4, 5,
        // 侧面
        0, 1, 4,  0, 4, 3,
        1, 2, 5,  1, 5, 4,
        2, 0, 3,  2, 3, 5
    };

    std::vector<GLint> colors(6, 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_p);
    glGenBuffers(1, &VBO_c);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 顶点坐标
    glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // 颜色
    glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLint), colors.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // 索引
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    this->indexCount = static_cast<GLsizei>(indices.size());
}

void edge::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
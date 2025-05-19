#include <cmath>

#include "geo_object.h"

const float r = 0.005;
const float r2 = 0.01;

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

        GLfloat tetrahedronColors[12] = { 0 };

        GLfloat expanded[36];
        for (int i = 0; i < 12; ++i) {
            int idx = indices[i];
            expanded [i * 3 + 0] = tetrahedronVertices[idx * 3 + 0];
            expanded [i * 3 + 1] = tetrahedronVertices[idx * 3 + 1];
            expanded [i * 3 + 2] = tetrahedronVertices[idx * 3 + 2];
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_p);
        glGenBuffers(1, &VBO_c);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(expanded), expanded, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tetrahedronColors), tetrahedronColors, GL_STATIC_DRAW);
        glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
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
        // 展开为 36 个顶点坐标
        GLfloat cubeVerticesExpanded[36 * 3];
        for (int i = 0; i < 36; ++i) {
            int idx = indices[i];
            cubeVerticesExpanded[i * 3 + 0] = cubeVertices[idx * 3 + 0];
            cubeVerticesExpanded[i * 3 + 1] = cubeVertices[idx * 3 + 1];
            cubeVerticesExpanded[i * 3 + 2] = cubeVertices[idx * 3 + 2];
        }
        GLfloat cubeColors[36] = { 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_p);
        glGenBuffers(1, &VBO_c);

        glBindVertexArray(VAO);

        // 绑定坐标数据到 VBO_p
        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesExpanded), cubeVerticesExpanded, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // 绑定颜色数据到 VBO_c
        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
        glVertexAttribIPointer(1, 1, GL_INT, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
}

void Point::draw() {
    glBindVertexArray(VAO);
    if (type) {
        glDrawArrays(GL_TRIANGLES, 0, 12 / 3);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
}

#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in int fragColor;
flat out int color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	color = fragColor;
	gl_Position = projection * view * vec4(aPos.y, aPos.x, aPos.z, 1.0);
}
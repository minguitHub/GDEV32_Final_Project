#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightModel, lightView, lightProjection;

void main()
{
	gl_Position = lightProjection * lightView * lightModel * vec4(aPos, 1.0f); // gl_Position is predefined output
};
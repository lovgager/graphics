#version 330 core

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec3 normal_in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 normal;
out vec3 position;

void main()
{
    normal = mat3(transpose(inverse(model))) * normal_in;
    position = vec3(model * vec4(position_in, 1.0f));
	
    gl_Position = projection * view * model * vec4(position_in, 1.0f);
}
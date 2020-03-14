#version 330 core

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec3 normal_in;
layout (location = 2) in vec2 texCoords_in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 position;
out vec3 normal;
out vec2 texCoords;
out vec4 lampSpacePosition;


void main()
{
    position = vec3(model * vec4(position_in, 1.0f));
    normal = transpose(inverse(mat3(model))) * normal_in;
    texCoords = texCoords_in;
    lampSpacePosition = lightSpaceMatrix * vec4(position, 1.0f);
	
    gl_Position = projection * view * model * vec4(position_in, 1.0f);
}
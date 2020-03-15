#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 color;

void main()
{
    color = vec4(0.58, 0.86, 0.04, 1.0);
}
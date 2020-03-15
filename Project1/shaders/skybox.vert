#version 330 core

layout (location = 0) in vec3 position_in;

uniform mat4 projection;
uniform mat4 view;

out vec3 texCoords;

void main()
{
    texCoords = position_in;
    vec4 pos = projection * view * vec4(position_in, 1.0f);
	
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}  
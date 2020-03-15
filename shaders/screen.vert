#version 330 core
layout (location = 0) in vec2 position_in;
layout (location = 1) in vec2 texCoords_in;

out vec2 texCoords;

void main()
{
    texCoords = texCoords_in;
	
    gl_Position = vec4(position_in.x, position_in.y, 0.0f, 1.0f); 
}  
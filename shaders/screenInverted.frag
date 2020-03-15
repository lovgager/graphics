#version 330 core

in vec2 texCoords;

uniform sampler2D screenTexture;

out vec4 color;

void main()
{
    color = vec4(vec3(1.0f - texture(screenTexture, texCoords)), 1.0f);
}   
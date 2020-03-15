#version 330 core

in vec3 normal;
in vec3 position;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

out vec4 color;

void main()
{    
    vec3 viewDirection = normalize(position - cameraPosition);
    vec3 reflection = reflect(viewDirection, normalize(normal));
	
    color = texture(skybox, reflection);
}
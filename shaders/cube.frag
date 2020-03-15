#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

uniform sampler2D ourTexture;  
uniform vec3 lampPosition; 

out vec4 color;

void main()
{
    vec3 ambient = vec3(0.2f);
  	
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(lampPosition - position);
    float coef = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = coef * vec3(1.0f);
    
    vec3 result = (ambient + diffuse) * vec3(texture(ourTexture, texCoords));
    color = vec4(result, 1.0f);
} 
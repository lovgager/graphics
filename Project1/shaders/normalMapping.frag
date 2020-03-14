#version 330 core

in vec3 position;
in vec2 texCoords;

uniform sampler2D ourTexture;
uniform sampler2D normalMap;
uniform vec3 lampPosition;

out vec4 color_out;

void main()
{           
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
   
    vec3 color = texture(ourTexture, texCoords).rgb;
    vec3 ambient = 0.1 * color;
    vec3 lightDirection = normalize(lampPosition - position);
    float coef = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = coef * color;
	
    vec3 reflectDirection = reflect(-lightDirection, normal);
    coef = pow(max(dot(normal, reflectDirection), 0.0), 32.0);
    vec3 specular = coef * vec3(0.2f);
	
    color_out = vec4(ambient + diffuse + specular, 1.0);
}
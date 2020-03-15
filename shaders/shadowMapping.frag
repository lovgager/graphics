#version 330 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texCoords;
in vec4 lampSpacePosition;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;

uniform vec3 lampPosition;

float ShadowCalculation(vec4 lampSpacePosition)
{
    vec3 normCoords = vec3(lampSpacePosition) * 0.5 + 0.5;
    float currentDepth = normCoords.z;
	
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, normCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - 0.05f > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(normCoords.z > 0.8)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(ourTexture, texCoords).rgb;
    vec3 normal = normalize(normal);
    vec3 lightColor = vec3(0.3f);

    vec3 ambient = 0.3f * color;
    vec3 lightDirection = normalize(lampPosition - position);
    float coef = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = coef * lightColor;
	
    float shadow = ShadowCalculation(lampSpacePosition);                      
    vec3 result = (ambient + (1.0f - shadow) * diffuse) * color;    
    
    FragColor = vec4(result, 1.0f);
}
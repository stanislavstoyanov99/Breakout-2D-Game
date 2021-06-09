#version 460 core

struct Material {
    sampler2D texture_diffuse1;
    float shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
};

out vec4 FragColor;
  
uniform vec3 viewPos;

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{    
    vec3 diffuseColor = vec3(texture(material.texture_diffuse1, TexCoords));

    // ambient
    vec3 ambient = light.ambient * diffuseColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    // combined
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
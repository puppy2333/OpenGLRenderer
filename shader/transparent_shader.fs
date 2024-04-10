#version 330 core

uniform vec3 viewPos;

out vec4 FragColor;

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

void main()
{
    vec4 diffuse_color = texture(material.diffuse, TextCoord);
    
    FragColor = diffuse_color;
}

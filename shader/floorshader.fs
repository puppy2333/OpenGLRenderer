#version 330 core

uniform vec3 viewPos;

out vec4 FragColor;

in vec2 TextCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform Light light;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TextCoord).rgb;
//    vec3 ambient = light.ambient * vec3(0.0, 0.0, 1.0);
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TextCoord).rgb;
//    vec3 diffuse = diff * light.diffuse * vec3(0.0, 0.0, 1.0);
    
    // Spectular
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Phong
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Blinn-Phong
    vec3 halfang = normalize(lightDir + viewDir / 2);
    float spec = pow(max(dot(norm, halfang), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;
    
    // Combined color
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}


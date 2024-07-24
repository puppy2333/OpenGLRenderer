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

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    vec4 diffuse_color = texture(material.diffuse, TextCoord);
    if(diffuse_color.a < 0.1)
        discard;
    
    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TextCoord).rgb;
//    vec3 ambient = light.ambient * vec3(1.0, 0.0, 0.0);
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TextCoord).rgb;
//    vec3 diffuse = diff * light.diffuse * vec3(1.0, 0.0, 0.0);
    
    // Spectular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * texture(material.specular, TextCoord).rgb;
//    vec3 specular = spec * light.specular * vec3(1.0, 0.0, 0.0);
    
    // Combined color
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}

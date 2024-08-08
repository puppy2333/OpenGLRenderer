#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 evalAmbient(vec3 color)
{
    return 0.3 * color;
}

vec3 evalDiffuse(vec3 lightDir, vec3 color, vec3 normal)
{
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    return diffuse;
}

vec3 evalSpecular(vec3 lightDir, vec3 color, vec3 normal)
{
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * color;
    return specular;
}

void main()
{
    vec3 color = vec3(0.0f, 0.0f, 0.7f);
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Direct lightning
    vec3 ambientBRDF = evalAmbient(color);
    vec3 diffuseBRDF = evalDiffuse(lightDir, color, normal);
    vec3 specularBRDF = evalSpecular(lightDir, color, normal);
    vec3 lighting = (ambientBRDF + diffuseBRDF + specularBRDF) * lightColor;
    
    FragColor = vec4(lighting, 1.0f);
}

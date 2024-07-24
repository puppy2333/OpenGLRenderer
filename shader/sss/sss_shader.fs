// Based on: GDC 2011 – Approximating Translucency for a Fast, Cheap and
// Convincing Subsurface Scattering Look
// URL: https://colinbarrebrisebois.com/2011/03/07/gdc-2011-approximating-
// translucency-for-a-fast-cheap-and-convincing-subsurface-scattering-look/
#version 330 core

#define PI 3.1415927
#define TWOPI 6.2831853
#define INV_PI 0.3183099
#define INV_TWOPI 0.1591549

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D ssaoColorBufferBlur;

uniform vec3 lightPos;
//uniform vec3 lightPosT;
uniform vec3 viewPos;

vec3 evalAmbient(vec3 color)
{
    return 0.3 * color;
}

vec3 evalDiffuse(vec3 lightDir, vec3 color, vec3 normal)
{
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * color * INV_PI;
    vec3 diffuse = diff * color;
    return diffuse;
}

void main()
{
    vec3 color = vec3(0.0f, 0.0f, 1.0f);
    vec3 normal = normalize(Normal);
    float thickness = texture(ssaoColorBufferBlur, TexCoords).r;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightColor = vec3(1.0);
    
    // Transparency
    float ambient = 0.3f;
    float ldir_distort = 0.01f;
    vec3 ldir_reflect = lightDir + normal * ldir_distort;
    //vec3 ldir_reflect = lightDir;
    float diff = max(dot(viewDir, -ldir_reflect), 0.0f);
    float diffuseBTDF = (diff + ambient) * thickness;
    vec3 lighting = color * lightColor * diffuseBTDF;
    
    FragColor = vec4(lighting, 1.0f);
}


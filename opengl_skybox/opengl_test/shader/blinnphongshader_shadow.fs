#version 330 core

#define PI 3.1415927
#define TWOPI 6.2831853
#define INV_PI 0.3183099
#define INV_TWOPI 0.1591549

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int imgui_shadowtype;

// --------------------------------------------
// ---------- For shadow calculation ----------
// --------------------------------------------
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);

    // Vanilla shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    return shadow;
}

float PCFShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // PCF (percentage-closer filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

float PCSSShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // PCSS (percentage colser soft shadows)
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    int count = 0;
    float dist = currentDepth - closestDepth;
    for(int x = -4; x <= 4; ++x) {
        for(int y = -4; y <= 4; ++y) {
            float neighborClosestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            if (currentDepth - bias > neighborClosestDepth) {
                dist += currentDepth - neighborClosestDepth;
                count++;
            }
        }
    }
    if (count == 0) {
        return 0;
    }
    float ave_dist = dist / count;
    
    int half_kernel_size = 0;
    if (ave_dist / currentDepth < 0.003) {
        half_kernel_size = 0;
    }
    else if (ave_dist / currentDepth < 0.005) {
        half_kernel_size = 1;
    }
    else if (ave_dist / currentDepth < 0.01) {
        half_kernel_size = 2;
    }
    else if (ave_dist / currentDepth < 0.015) {
        half_kernel_size = 3;
    }
    else if (ave_dist / currentDepth < 0.02) {
        half_kernel_size = 4;
    }
    else if (ave_dist / currentDepth < 0.025) {
        half_kernel_size = 5;
    }
    else {
        half_kernel_size = 6;
    }
        
    float shadow = 0.0;
    for(int x = -half_kernel_size; x <= half_kernel_size; ++x)
    {
        for(int y = -half_kernel_size; y <= half_kernel_size; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (2 * half_kernel_size + 1) * (2 * half_kernel_size + 1);
    
    return shadow;
}

// ------------------------------------------
// ---------- For direct lightning ----------
// ------------------------------------------
vec3 evalAmbient(vec3 color)
{
    return 0.15 * color;
}

vec3 evalDiffuse(vec3 lightDir, vec3 color, vec3 normal)
{
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * color * INV_PI;
    vec3 diffuse = diff * color;
    return diffuse;
}

vec3 evalSpecular(vec3 lightDir, vec3 color, vec3 normal)
{
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * color;
    return specular;
}

vec3 evalDirectLight(vec3 lightColor, vec3 lightDir, vec3 normal)
{
    // Calculate shadow
    float shadow = 0;
    if (imgui_shadowtype == 0) {
        shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    }
    else if (imgui_shadowtype == 1) {
        shadow = PCFShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    }
    else if (imgui_shadowtype == 2) {
        shadow = PCSSShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    }
    return (1.0 - shadow) * lightColor;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    
    // Direct lightning
    vec3 ambientBRDF = evalAmbient(color);
    vec3 diffuseBRDF = evalDiffuse(lightDir, color, normal);
    // vec3 specularBRDF = evalSpecular(lightDir, color, normal);
    vec3 directLight =evalDirectLight(lightColor, lightDir, normal);
    // vec3 lighting = ambientBRDF * lightColor + (diffuseBRDF + specularBRDF) * directLight;
    vec3 lighting = ambientBRDF * lightColor + diffuseBRDF * directLight;
    // vec3 lighting = diffuseBRDF * directLight;
    
    FragColor = vec4(lighting, 1.0f);
}

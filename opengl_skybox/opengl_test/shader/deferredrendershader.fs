#version 330 core

#define PI 3.1415927
#define TWOPI 6.2831853
#define INV_PI 0.3183099
#define INV_TWOPI 0.1591549

out vec4 FragColor;

in vec2 TexCoords;

// G buffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gShadow;

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

vec3 evalDirectLight(vec3 lightColor, vec3 lightDir, vec3 normal, float shadow)
{
    return (1.0 - shadow) * lightColor;
    return lightColor;
}

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    float shadow = texture(gShadow, TexCoords).r;
    
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Materials
    vec3 ambientBRDF = evalAmbient(color);
    vec3 diffuseBRDF = evalDiffuse(lightDir, color, normal);
    
    // Direct lightning
    vec3 directLight = evalDirectLight(lightColor, lightDir, normal, shadow);
    
    // Combined
    vec3 lighting = ambientBRDF * lightColor + diffuseBRDF * directLight;
    FragColor = vec4(lighting, 1.0f);
    
//    FragColor = vec4(lighting, 1.0f);
//    FragColor = vec4(fragPos, 1.0f);
//    FragColor = vec4(color, 1.0f);
//    FragColor = vec4(normal, 1.0f);
//    FragColor = vec4(shadow, shadow, shadow, 1.0f);
    
    // ---------- Test: depth map ----------
    // float depthmap = texture(shadowMap, TexCoords).r;
    // FragColor = vec4(depthmap, depthmap, depthmap, 1.0f);
}

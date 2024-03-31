#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gShadow;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int imgui_shadowtype;

// --------------------------------------------
// ---------- For shadow calculation ----------
// --------------------------------------------
float ShadowCalculation()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);

    // Vanilla shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    return shadow;
}

float PCFShadowCalculation()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
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

float PCSSShadowCalculation()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
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

void main()
{
    gPosition.rgb = FragPos;
    gPosition.a = 1.0f;
    
    gNormal.rgb = normalize(Normal);
    gNormal.a = 1.0f;
    
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = 1.0f;
    
    float shadow = 0.0f;
    if (imgui_shadowtype == 0) {
        shadow = ShadowCalculation();
    }
    else if (imgui_shadowtype == 1) {
        shadow = PCFShadowCalculation();
    }
    else if (imgui_shadowtype == 2) {
        shadow = PCSSShadowCalculation();
    }
    
    gShadow.r = shadow;
//    gShadow.r = 1.0f;
    gShadow.g = gl_FragCoord.z;
    gShadow.b = 1.0f;
    gShadow.a = 1.0f;
    //gShadow.gba = vec3(1.0f);
}

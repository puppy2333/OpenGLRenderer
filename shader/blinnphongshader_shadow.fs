// About PCF and PCSS
// General report: Percentage-Closer Soft Shadows (Nvidia)
// URL: https://developer.download.nvidia.com/shaderlibrary/docs/shadow_PCSS.pdf
// The implementation refer to GAMES 202

#version 330 core

#define PI 3.1415927
#define TWO_PI 6.2831853
#define INV_PI 0.3183099
#define INV_TWO_PI 0.1591549

// Poisson disk sample
#define NUM_SAMPLES 64
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

#define POISSON_RADIUS 10
#define BLOCKER_SEARCH_POISSON_RADIUS POISSON_RADIUS
#define PCF_POISSON_RADIUS 2 * POISSON_RADIUS

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

// Global list for poisson disk samples
vec2 poissonDisk[NUM_SAMPLES];

// Hw1 of GAMES 202
float rand_1to1(float x) {
    // -1 -1
    return fract(sin(x)*10000.0);
}

float rand_2to1(vec2 uv) {
    // 0 - 1
	const float a = 12.9898, b = 78.233, c = 43758.5453;
	float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

void poissonDiskSamples(const in vec2 randomSeed) {
    float ANGLE_STEP = TWO_PI * float(NUM_RINGS) / float( NUM_SAMPLES );
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(randomSeed) * TWO_PI;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

void uniformDiskSamples(const in vec2 randomSeed) {
    float randNum = rand_2to1(randomSeed);
    float sampleX = rand_1to1( randNum ) ;
    float sampleY = rand_1to1( sampleX ) ;

    float angle = sampleX * TWO_PI;
    float radius = sqrt(sampleY);

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2(radius * cos(angle), radius * sin(angle));

        sampleX = rand_1to1( sampleY );
        sampleY = rand_1to1( sampleX );

        angle = sampleX * TWO_PI;
        radius = sqrt(sampleY);
    }
}

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

float PCFShadowCalculationPoissonDiskSample(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF (percentage-closer filtering)
    poissonDiskSamples(projCoords.xy);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec2 coord = projCoords.xy + poissonDisk[i] * texelSize * POISSON_RADIUS;
        float pcfDepth = texture(shadowMap, coord).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= NUM_SAMPLES;

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

float PCSSShadowCalculationPoissonDiskSample(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCSS (percentage colser soft shadows)
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    int count = 0;
    float averageNeighborDepth = 0;

    poissonDiskSamples(projCoords.xy);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec2 coord = projCoords.xy + poissonDisk[i] * texelSize * PCF_POISSON_RADIUS;

        float neighborDepth = texture(shadowMap, coord).r;
        if (currentDepth - bias > neighborDepth) {
            averageNeighborDepth += neighborDepth;
            count++;
        }
    }
    if (count == 0) {
        return 0;
    }
    averageNeighborDepth = averageNeighborDepth / count;

    // 800 is a quite large number, the reason is that the light is too far away currently
    float half_kernel_size = (currentDepth - averageNeighborDepth) / averageNeighborDepth * 800.0;

    float shadow = 0.0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec2 coord = projCoords.xy + poissonDisk[i] * texelSize * half_kernel_size;
        float pcfDepth = texture(shadowMap, coord).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= NUM_SAMPLES;

    return shadow;
}

// ------------------------------------------
// ---------- For direct lightning ----------
// ------------------------------------------
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
        shadow = 0;
    }
    if (imgui_shadowtype == 1) {
        shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    }
    else if (imgui_shadowtype == 2) {
        shadow = PCFShadowCalculationPoissonDiskSample(fs_in.FragPosLightSpace, normal, lightDir);
    }
    else if (imgui_shadowtype == 3) {
        shadow = PCSSShadowCalculationPoissonDiskSample(fs_in.FragPosLightSpace, normal, lightDir);
    }
    return (1.0 - shadow) * lightColor;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.5);
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

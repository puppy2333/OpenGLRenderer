#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gShadow;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

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

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int imgui_shadowtype;

uniform int is_mirror;

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

float PCFShadowCalculationPoissonDiskSample()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
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

float PCSSShadowCalculationPoissonDiskSample()
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

void main()
{
    gPosition.rgb = FragPos;
    gPosition.a = 1.0f;
    
    gNormal.rgb = normalize(Normal);
    gNormal.a = 1.0f;
    
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = 1.0f;
    // gAlbedoSpec.a = texture(texture_diffuse1, TexCoords).a;
    
    float shadow = 0.0f;
    if (imgui_shadowtype == 0) {
        shadow = 0.0f;
    }
    if (imgui_shadowtype == 1) {
        shadow = ShadowCalculation();
    }
    else if (imgui_shadowtype == 2) {
        shadow = PCFShadowCalculationPoissonDiskSample();
    }
    else if (imgui_shadowtype == 3) {
        shadow = PCSSShadowCalculationPoissonDiskSample();
    }
    
    gShadow.r = shadow;
    gShadow.g = gl_FragCoord.z;
    if (is_mirror == 1) {
        gShadow.b = 1.0f;
    }
    else {
        gShadow.b = 0.0f;
    }
    gShadow.a = 1.0f;
    //gShadow.gba = vec3(1.0f);
}

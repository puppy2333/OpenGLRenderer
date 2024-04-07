#version 330 core
//out float FragColor;
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gShadow;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];

int kernelSize = 16;
float radius = 2.0;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1600.0f/4.0f, 1200.0f/4.0f);

uniform mat4 view;
uniform mat4 projection;

float LinearizeDepth(float depth)
{
    float NEAR = 1.0f;
    float FAR = 40.0f;
    
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(noiseTexture, TexCoords * noiseScale).xyz;
    float depth = texture(gShadow, TexCoords).y;
    depth = LinearizeDepth(depth);
    
    // Gram-Schmidt orthogonal
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    mat3 TBN = mat3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 mysample = TBN * samples[i];
        mysample = fragPos + mysample * radius;
        
        // project sample position
        vec4 offset = vec4(mysample, 1.0);
        offset = projection * view * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // get sample depth
        float sampleDepth = texture(gShadow, offset.xy).y;
        sampleDepth = LinearizeDepth(sampleDepth);
        
        // range check & accumulate
        float rangeCheck = smoothstep(1.0, 0.0, abs(depth - sampleDepth) / radius);
        if (abs(depth - sampleDepth) < 0.1)
            rangeCheck = 0;
        occlusion += (sampleDepth <= depth ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    //occlusion /= kernelSize;
    
    FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
    //FragColor = occlusion;
}

//void main()
//{
//    // Get input for SSAO algorithm
//    vec3 fragPos = texture(gPosition, TexCoords).xyz;
//    vec3 normal = texture(gNormal, TexCoords).rgb;
//    vec3 randomVec = texture(noiseTexture, TexCoords * noiseScale).xyz;
//    float depth = texture(gShadow, TexCoords).y;
//    depth = LinearizeDepth(depth);
//    
//    // Gram-Schmidt orthogonal
//    //vec4 temp = view * vec4(normal, 1.0f);
//    //normal = temp.xyz;
//    
//    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
//    vec3 bitangent = cross(normal, tangent);
//    
//    // Create TBN change-of-basis matrix: from tangent-space to view-space
//    mat3 TBN = mat3(tangent, bitangent, normal);
//    // Iterate over the sample kernel and calculate occlusion factor
//    float occlusion = 0.0;
//    for (int i = 0; i < 64; ++i)
//    {
//        // get sample position
//        vec3 mysample = TBN * samples[i];
//        mysample = fragPos + mysample * radius;
//        
//        // project sample position
//        vec4 offset = vec4(mysample, 1.0);
//        //offset = projection * view * offset;
//        offset = projection * offset;
//        offset.xyz /= offset.w;
//        offset.xyz = offset.xyz * 0.5 + 0.5;
//        
//        // get sample depth
//        float sampleDepth = texture(gShadow, offset.xy).y;
//        sampleDepth = LinearizeDepth(sampleDepth);
//        
//        // range check & accumulate
//        float rangeCheck = smoothstep(1.0, 0.0, abs(depth - sampleDepth) / radius);
//        occlusion += (sampleDepth >= depth ? 1.0 : 0.0) * rangeCheck;
//    }
//    //occlusion = 1.0 - (occlusion / kernelSize);
//    occlusion /= kernelSize;
//    
//    FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
//}

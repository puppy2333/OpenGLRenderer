#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D depthMap;

//void main()
//{
//    float depthValue = texture(depthMap, TexCoords).r;
//    color = vec4(vec3(depthValue), 1.0);
//}

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float far_plane = 20.0f;
    float near_plane = 4.0f;
    
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float far_plane = 20.0f;
    float near_plane = 4.0f;
    
    float depthValue = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    // FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}

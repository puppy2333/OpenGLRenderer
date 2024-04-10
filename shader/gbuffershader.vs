#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// For shadowmap
uniform mat4 lightProjection;
uniform mat4 lightView;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // Position
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;
    
    // Normal
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;
    
    // Light space fragment position (for shadowmap)
    FragPosLightSpace = lightProjection * lightView * vec4(FragPos, 1.0);

    gl_Position = projection * view * worldPos;
}

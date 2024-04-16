#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

uniform sampler2D heightmap;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // Constants
    float dx = 0.01;
    
    // Height
    float sampled_h = texture(heightmap, texCoords).z;

    vec3 shifted_pos = position;
    shifted_pos.z += sampled_h;
    
    gl_Position = projection * view * model * vec4(shifted_pos, 1.0f);
    TexCoords = texCoords;
    FragPos = vec3(model * vec4(shifted_pos, 1.0f));
    
    // Normal
    float sampled_h_r = texture(heightmap, vec2(texCoords.x + 0.5 * dx, texCoords.y)).z;
    float sampled_h_l = texture(heightmap, vec2(texCoords.x - 0.5 * dx, texCoords.y)).z;
    
    float sampled_h_t = texture(heightmap, vec2(texCoords.x, texCoords.y + 0.5 * dx)).z;
    float sampled_h_b = texture(heightmap, vec2(texCoords.x, texCoords.y - 0.5 * dx)).z;
    
    vec3 vec_1 = normalize(vec3(dx, sampled_h_r - sampled_h_l, 0.0f));
    vec3 vec_2 = normalize(vec3(0.0f, sampled_h_t - sampled_h_b, dx));
    
    vec3 normal = -normalize(cross(vec_1, vec_2));
    Normal = normal;
    
//    gl_Position = projection * view * model * vec4(position, 1.0f);
//    TexCoords = texCoords;
}



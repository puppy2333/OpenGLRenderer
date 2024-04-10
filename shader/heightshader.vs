#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

uniform sampler2D heightmap;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    float sampled_heightmap = texture(heightmap, texCoords).x;
 
    vec3 myposition = position;
    myposition.z += sampled_heightmap;
    
    gl_Position = projection * view * model * vec4(myposition, 1.0f);
    TexCoords = texCoords;
}



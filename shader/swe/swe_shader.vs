#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 1.0f);
    
    // I don't know why
    TexCoords.x = texCoords.x * 16;
    TexCoords.y = texCoords.y * 12;
}



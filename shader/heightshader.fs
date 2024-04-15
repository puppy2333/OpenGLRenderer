#version 330 core
//out float FragColor;
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D heightmap;

void main()
{
    float s = texture(heightmap, TexCoords).z * 10;
    
    FragColor = vec4(s, s, s, 1.0f);
    
//    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}


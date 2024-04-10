#version 330 core
//out float FragColor;
out vec4 FragColor;
in vec2 TexCoords;

void main()
{
    FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0);
}

#version 330 core
//out float FragColor;
out vec4 FragColor;
in vec2 TexCoords;

void main()
{
    //float xh = fract(TexCoords.x * TexCoords.y * 4.52342);
    //float yh = fract(TexCoords.x * TexCoords.y * 7.43853);
    
    float xh = 1 - (TexCoords.x - 0.5) * (TexCoords.x - 0.5) * 2 - (TexCoords.y - 0.5) * (TexCoords.y - 0.5) * 2;
    
    //FragColor = vec4(TexCoords.x, 0.0, 0.0, 1.0);
    //FragColor = vec4(0.2 * xh, 0.0, 0.0, 1.0);
    FragColor = vec4(xh, 0.0, 0.0, 1.0);
}

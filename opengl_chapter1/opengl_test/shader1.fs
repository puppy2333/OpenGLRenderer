#version 330 core

out vec4 FragColor;

//in vec4 ourColor;
in vec2 TextCoord;

uniform sampler2D ourTexture;

//uniform vec4 ourColor;

void main()
{
    // FragColor = ourColor;
    FragColor = texture(ourTexture, TextCoord);
//    FragColor = vec4(100, 100, 100, 1);
}

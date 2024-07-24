#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sweBuffer1;

void main()
{
    // Sample
    vec3 vel_h = texture(sweBuffer1, TexCoords).xyz;
    
    FragColor = vec4(vel_h, 1.0f);
}





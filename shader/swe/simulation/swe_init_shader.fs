#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sweBuffer2;

void main()
{
    vec3 vel_h = texture(sweBuffer2, TexCoords).xyz;
    if ((TexCoords.x - 0.5) * (TexCoords.x - 0.5) + (TexCoords.y - 0.5) * (TexCoords.y - 0.5) < 0.01) {
        FragColor = vec4(vel_h.xy, 0.1f, 1.0f);
    }
    else {
        FragColor = vec4(vel_h, 1.0f);
    }
}




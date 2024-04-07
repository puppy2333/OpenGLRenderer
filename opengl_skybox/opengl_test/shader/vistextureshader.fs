#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

// G buffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gShadow;

void main()
{
    vec3 tex = texture(gPosition, TexCoords).rgb;
    FragColor = vec4(tex, 1.0f);
}

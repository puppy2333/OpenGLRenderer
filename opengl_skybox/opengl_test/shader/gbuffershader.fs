#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

void main()
{
    gPosition.rgb = FragPos;
    gPosition.a = 1.0f;
    gNormal.rgb = normalize(Normal);
    gNormal.a = 1.0f;
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = 1.0f;
}

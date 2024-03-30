#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    
//    FragColor = vec4(lighting, 1.0f);
    FragColor = vec4(fragPos, 1.0f);
//    FragColor = vec4(color, 1.0f);
//    FragColor = vec4(normal, 1.0f);
}

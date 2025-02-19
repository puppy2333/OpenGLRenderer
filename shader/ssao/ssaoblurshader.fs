#version 330 core
//out float FragColor;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoColorBuffer;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoColorBuffer, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoColorBuffer, TexCoords + offset).r;
        }
    }
    // FragColor = result / (4.0 * 4.0);
    result = result / (4.0 * 4.0);
    FragColor = vec4(result, result, result, 1.0f);
    //FragColor = vec4(result, 0.0f, 0.0f, 1.0f);
    //FragColor = result;
}

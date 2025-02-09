#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 texture_coords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.frag_pos = vec3(model * vec4(position, 1.0));
    // vs_out.normal = transpose(inverse(mat3(model))) * normal;
    vs_out.normal = normal;
    vs_out.texture_coords = texture_coords;
}

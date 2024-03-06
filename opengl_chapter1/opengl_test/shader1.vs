#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTextCoord;
layout (location = 1) in vec2 aTextCoord;

//out vec4 ourColor;
out vec2 TextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
//    gl_Position = trans * vec4(aPos.x, aPos.y, aPos.z, 1.0);
//    gl_Position = trans * vec4(aPos, 1.0);
//    gl_Position = vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
//    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
//    ourColor = vec4(aColor, 1.0f);
    TextCoord = aTextCoord;
}

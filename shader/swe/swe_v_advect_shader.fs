#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sweBuffer2;

// Mac grid layout:
//          uy(0, 2)          uy(1, 2)
// ux(0, 1)  h(0, 1) ux(1, 1)  h(1, 1) ux(2, 1)
//          uy(0, 1)          uy(1, 1)
// ux(0, 0)  h(0, 0) ux(1, 0)  h(1, 0) ux(2, 0)
//          uy(0, 0)          uy(1, 0)
void main()
{
    // Constants
    float dt = 0.005;
    float dx = 0.01;
    
    // Sample
    vec2 u = texture(sweBuffer2, TexCoords).xy;
    float h = texture(sweBuffer2, TexCoords).z;
    
    // Advect ux
    float uy_atux = texture(sweBuffer2, vec2(TexCoords.x - 0.5 * dx, TexCoords.y + 0.5 * dx)).y;
    vec2 backtrace_pos_atux = TexCoords - vec2(u.x, uy_atux) * dt;
    float new_ux = texture(sweBuffer2, backtrace_pos_atux).x;
    
    // Advect uy
    float ux_atuy = texture(sweBuffer2, vec2(TexCoords.x + 0.5 * dx, TexCoords.y - 0.5 * dx)).x;
    vec2 backtrace_pos_atuy = TexCoords - vec2(ux_atuy, u.y) * dt;
    float new_uy = texture(sweBuffer2, backtrace_pos_atuy).y;
    
    FragColor = vec4(new_ux, new_uy, h, 1.0f);
}

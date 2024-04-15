#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sweBuffer1;

void main()
{
    // Constants
    float dt = 0.005;
    float dx = 0.01;
    
    // Sample
    vec2 vel = texture(sweBuffer1, TexCoords).xy;
    float h = texture(sweBuffer1, TexCoords).z;
        
    float ux_r = texture(sweBuffer1, vec2(TexCoords.x + dx, TexCoords.y)).x;
    float ux_l = texture(sweBuffer1, vec2(TexCoords.x - dx, TexCoords.y)).x;
    float uy_t = texture(sweBuffer1, vec2(TexCoords.x, TexCoords.y + dx)).y;
    float uy_b = texture(sweBuffer1, vec2(TexCoords.x, TexCoords.y - dx)).y;
    
    float h_r = texture(sweBuffer1, vec2(TexCoords.x + dx, TexCoords.y)).z;
    float h_l = texture(sweBuffer1, vec2(TexCoords.x - dx, TexCoords.y)).z;
    float h_t = texture(sweBuffer1, vec2(TexCoords.x, TexCoords.y + dx)).z;
    float h_b = texture(sweBuffer1, vec2(TexCoords.x, TexCoords.y - dx)).z;
    
    float h_r_bar = ux_r <= 0 ? h_r : h;
    float h_l_bar = vel.x <= 0 ? h : h_l;
    float h_t_bar = uy_t <= 0 ? h_t : h;
    float h_b_bar = vel.y <= 0 ? h : h_b;
    
    // Integrate h
    float dh = -(dt / dx) * ((h_r_bar * ux_r - h_l_bar * vel.x) + ( h_t_bar * uy_t - h_b_bar * vel.y));
    
    FragColor = vec4(vel, h + dh, 1.0f);
}


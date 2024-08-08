#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sweBuffer2;

void main()
{
    // Constants
    float dt = 0.005;
    float dx = 0.01;
    
    float g = 10;
    //float max_vel = 0.005 * (dx / dt);
    
    float scale = (dt / dx) * g;

    // Integrate v
    vec2 vel = texture(sweBuffer2, TexCoords).xy;
    float h = texture(sweBuffer2, TexCoords).z;
    float h_l = texture(sweBuffer2, vec2(TexCoords.x - dx, TexCoords.y)).z; // h on the left
    float h_b = texture(sweBuffer2, vec2(TexCoords.x, TexCoords.y - dx)).z; // h on the bottom

    vec2 new_vel;
    
    // velocity x
    float delta_vx = - scale * (h - h_l);
    new_vel.x = vel.x + delta_vx;
    //new_vel.x = clamp(new_vel.x, -max_vel, max_vel);

    // velocity y
    float delta_vy = - scale * (h - h_b);
    new_vel.y = vel.y + delta_vy;
    //new_vel.y = clamp(new_vel.y, -max_vel, max_vel);
    
    // Temporary boundary handling
    FragColor = vec4(new_vel, h, 1.0f);
    if (TexCoords.x <= 0.01 || TexCoords.y <= 0.01)
        FragColor = vec4(0.0f, 0.0f, h, 1.0f);
    if (TexCoords.x >= 0.99 || TexCoords.y >= 0.99)
        FragColor = vec4(0.0f, 0.0f, h, 1.0f);
}



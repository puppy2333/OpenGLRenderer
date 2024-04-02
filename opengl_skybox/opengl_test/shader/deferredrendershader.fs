#version 330 core

#define PI 3.1415927
#define TWOPI 6.2831853
#define INV_PI 0.3183099
#define INV_TWOPI 0.1591549

out vec4 FragColor;

in vec2 TexCoords;

// G buffers
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gShadow;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 VPMatrix;

uniform int numray;

// Direct lightning
// ----------------
vec3 evalAmbient(vec3 color)
{
    return 0.15 * color;
}

vec3 evalDiffuse(vec3 lightDir, vec3 color, vec3 normal)
{
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * INV_PI;
    return diffuse;
}

vec3 evalDirectLight(vec3 lightColor, float shadow)
{
    return (1.0 - shadow) * lightColor;
    return lightColor;
}

// Indirect lightning
// ------------------

// Helper functions from games202 hw3
// –---------------------------------
float InitRand(vec2 uv) {
    vec3 p3  = fract(vec3(uv.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float Rand1(float p) {
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

vec2 Rand2(float p) {
    return vec2(Rand1(p), Rand1(p));
}

vec3 SampleHemisphereUniform(float s) {
    vec2 uv = Rand2(s);
    float z = uv.x;
    float phi = uv.y * TWOPI;
    float sinTheta = sqrt(1.0 - z*z);
    vec3 dir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), z);
    return dir;
}

vec3 LocalBasis_b1(vec3 n) {
    float sign_ = sign(n.z);
    if (n.z == 0.0) {
        sign_ = 1.0;
    }
    float a = -1.0 / (sign_ + n.z);
    float b = n.x * n.y * a;
    vec3 b1 = vec3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
    return b1;
}

vec3 LocalBasis_b2(vec3 n) {
    float sign_ = sign(n.z);
    if (n.z == 0.0) {
        sign_ = 1.0;
    }
    float a = -1.0 / (sign_ + n.z);
    float b = n.x * n.y * a;
    vec3 b2 = vec3(b, sign_ + n.y * n.y * a, -n.y);
    return b2;
}

// Get depth from camera view
float getDepth(vec3 pos_world) {
    vec4 pos_screen = VPMatrix * vec4(pos_world, 1.0);
    float depth = pos_screen.z / pos_screen.w * 0.5 + 0.5; 
    return depth;
}

// Perspective division
vec4 Project(vec4 a) {
  return a / a.w;
}

// Position to screen space
vec2 GetScreenCoord(vec3 pos_world) {
    vec2 uv = Project(VPMatrix * vec4(pos_world, 1.0)).xy * 0.5 + 0.5;
    return uv;
}

// GbufferDepth needs to be added
float GetGBufferDepth(vec2 uv) {
    float depth = texture(gShadow, uv).y;
    if (depth < 1e-2) {
        depth = 1000.0;
    }
    return depth;
}
// –---------------------------------

vec3 rayMarch(vec3 origin_point, vec3 ray_dir) {
    float dx = 0.05;
    vec3 end_point = origin_point;
    
    for (int i = 0; i < 80; i++) {
        vec3 test_point = end_point + dx * ray_dir;
        float test_point_depth = getDepth(test_point);
        float buffer_depth = GetGBufferDepth(GetScreenCoord(test_point));
        if (test_point_depth > buffer_depth + 1e-6) {
            return test_point;
        } else {
            end_point = test_point;
        }
    }
    return vec3(-1, -1, -1);
}

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    float shadow = texture(gShadow, TexCoords).r;
    float depth = texture(gShadow, TexCoords).g;
    
    if (depth < 1.0f) {
        vec3 lightColor = vec3(1.0);
        vec3 lightDir = normalize(lightPos - fragPos);
        
        // Materials
        // vec3 ambientBRDF = evalAmbient(color);
        vec3 diffuseBRDF = evalDiffuse(lightDir, color, normal);
        
        // Direct lightning
        vec3 directLight = evalDirectLight(lightColor, shadow);
        
        // vec3 L = ambientBRDF * lightColor + diffuseBRDF * directLight;
        vec3 L = diffuseBRDF * directLight;
        
        // Indirect lightning
        float rand_num = InitRand(gl_FragCoord.xy);
        
        vec3 L_indirect = vec3(0.0f, 0.0f, 0.0f);
        
        int num_ray = 2;
        if (numray == 1) num_ray = 4;
        else if (numray == 2) num_ray = 8;
        else if (numray == 3) num_ray = 16;
        
        for (int i = 0; i < num_ray; i++) {
            // Sample a light
            float pdf = INV_TWOPI;
            rand_num = Rand1(rand_num);
            vec3 sampled_dir = SampleHemisphereUniform(rand_num);
            
            // Construct local coordinate
            vec3 b1 = LocalBasis_b1(normal);
            vec3 b2 = LocalBasis_b2(normal);
            
            sampled_dir = normalize(mat3(b1, b2, normal) * sampled_dir);
            
            // Ray trace
            vec3 hit_pos = rayMarch(fragPos, sampled_dir);
            if (abs(hit_pos.x + 1) > 0.0001) {
                vec2 uv_new = GetScreenCoord(hit_pos);
                vec3 color_new = texture(gAlbedoSpec, uv_new).rgb;
                vec3 normal_new = texture(gNormal, uv_new).rgb;
                float shadow_new = texture(gShadow, uv_new).r;
                vec3 lightdir_new = normalize(lightPos - hit_pos);

                L_indirect += evalDiffuse(sampled_dir, color, normal) / (pdf) * evalDiffuse(lightdir_new, color_new, normal_new) * evalDirectLight(lightColor, shadow_new);
                L_indirect += evalDiffuse(lightdir_new, color_new, normal_new);
            }
        }
        L_indirect = L_indirect / float(num_ray);
        
        // Combined
        L += L_indirect;
        
        FragColor = vec4(L, 1.0f);
    }
    else {
        FragColor = vec4(0.2f, 0.3f, 0.3f, 1.0f);
    }
}


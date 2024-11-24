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
uniform sampler2D ssaoColorBufferBlur;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 VPMatrix;

uniform int numray;
uniform bool AO;

// Direct lightning
// ----------------
vec3 evalAmbient(vec3 color, float ao)
{
    return 0.3 * ao * color;
}

vec3 evalDiffuse(vec3 lightDir, vec3 color, vec3 normal)
{
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * color * INV_PI;
    vec3 diffuse = diff * color;
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
    return depth;
}
// –---------------------------------

bool is_uv_outofscreen(vec2 uv) {
    if (uv.x >= 1.0 || uv.x <= 0.0 || uv.y >= 1.0 || uv.y <= 0.0) {
        return true;
    }
    else {
        return false;
    }
}

float LinearizeDepth(float depth)
{
    float NEAR = 1.0f;
    float FAR = 40.0f;
    
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

vec3 rayMarch(vec3 origin_point, vec3 ray_dir) {
    float dx = 0.05;
    vec3 end_point = origin_point + 0.2 * ray_dir;
    
    for (int i = 0; i < 100; i++) {
        vec3 test_point = end_point + dx * ray_dir;
        float test_point_depth = getDepth(test_point);
        
        vec2 test_point_uv = GetScreenCoord(test_point);
        if (is_uv_outofscreen(test_point_uv) == true) {
            return vec3(-1000, -1000, -1000);
        }
        
        float buffer_depth = GetGBufferDepth(test_point_uv);
        
        // Linearize depth
        test_point_depth = LinearizeDepth(test_point_depth);
        buffer_depth = LinearizeDepth(buffer_depth);
        
        if (test_point_depth > buffer_depth + 1e-6 && test_point_depth < buffer_depth + 0.15) {
            return test_point;
        } else {
            end_point = test_point;
        }
    }
    return vec3(-1000, -1000, -1000);
}

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    float shadow = texture(gShadow, TexCoords).r;
    float depth = texture(gShadow, TexCoords).g;
    float is_mirror = texture(gShadow, TexCoords).b;
    float ao = 1.0f;
    if (AO) {
        ao = texture(ssaoColorBufferBlur, TexCoords).r;
    }
    
    vec3 lightColor = vec3(1.5);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    if (depth < 1.0f) {
        
        vec3 L = vec3(0.0f);
        
        if (is_mirror < 0.01) {
            // Materials
            vec3 ambientBRDF = evalAmbient(color, ao);
            //vec3 ambientBRDF = vec3(ao);
            vec3 diffuseBRDF = evalDiffuse(lightDir, color, normal);
            //vec3 diffuseBRDF = vec3(0.0f);
            
            // Direct lightning
            vec3 directLight = evalDirectLight(lightColor, shadow);
            
            L = ambientBRDF * lightColor + diffuseBRDF * directLight;
        }
        else {
            // Sample a light
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 dir_new = reflect(-viewDir, normal);
            
            // Ray trace
            vec3 hit_pos = rayMarch(fragPos, dir_new);
            if (abs(hit_pos.x + 1000) > 0.0001) {
                vec2 uv_new = GetScreenCoord(hit_pos);
                vec3 color_new = texture(gAlbedoSpec, uv_new).rgb;
                vec3 normal_new = texture(gNormal, uv_new).rgb;
                float shadow_new = texture(gShadow, uv_new).r;
                vec3 lightdir_new = normalize(lightPos - hit_pos);
                
                L = evalDiffuse(lightdir_new, color_new, normal_new) * evalDirectLight(lightColor, shadow_new);
                //L = vec3(1.0f);
            }
        }
        
        FragColor = vec4(L, 1.0f);
    }
    else {
        FragColor = vec4(0.2f, 0.3f, 0.3f, 1.0f);
    }
}



//        else {
//            for (int i = 0; i < numray; i++) {
//                // Sample a light
//                float pdf = INV_TWOPI;
//                rand_num = Rand1(rand_num);
//                vec3 sampled_dir = SampleHemisphereUniform(rand_num);
//
//                // Construct local coordinate
//                vec3 b1 = LocalBasis_b1(normal);
//                vec3 b2 = LocalBasis_b2(normal);
//
//                sampled_dir = normalize(mat3(b1, b2, normal) * sampled_dir);
//
//                // Ray trace
//                vec3 hit_pos = rayMarch(fragPos, sampled_dir);
//                if (abs(hit_pos.x + 1) > 0.0001) {
//                    vec2 uv_new = GetScreenCoord(hit_pos);
//                    vec3 color_new = texture(gAlbedoSpec, uv_new).rgb;
//                    vec3 normal_new = texture(gNormal, uv_new).rgb;
//                    float shadow_new = texture(gShadow, uv_new).r;
//                    vec3 lightdir_new = normalize(lightPos - hit_pos);
//
//                    L_indirect += evalDiffuse(sampled_dir, color, normal) / (pdf) * evalDiffuse(lightdir_new, color_new, normal_new) * evalDirectLight(lightColor, shadow_new);
//                }
//            }
//        }

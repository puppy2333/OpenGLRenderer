#version 330 core
out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 texture_coords;
} fs_in;

uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform vec3 light_color;

uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

#define PI 3.1415927

/* Schlick approximation of Fresnel equation
*/
vec3 fresnel_schlick(vec3 N, vec3 V, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - dot(N, V), 5.0);
}

/* Normal distribution function
*/
float normal_distribution_trowbridge_reitz_ggx(vec3 N, vec3 H, float roughness)
{
    float alpha2 = roughness * roughness;
    float NdotH  = dot(N, H);
    float NdotH2 = NdotH * NdotH;

    float num = alpha2;
    float denom = (NdotH2 * (alpha2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

/* Geometry function
*/
float geometry_schlick_ggx(vec3 N, vec3 V, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float k = pow(roughness + 1.0, 2) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

/* Smith's method for combining geometry obstruction and geometry shadowing
*/
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float ggx_obstruction = geometry_schlick_ggx(N, V, roughness);
    float ggx_shadowing = geometry_schlick_ggx(N, L, roughness);

    return ggx_obstruction * ggx_shadowing;
    // return ggx_obstruction;
    // return ggx_shadowing;
}

void main()
{
    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(cam_pos - fs_in.frag_pos);

    vec3 L = normalize(light_pos - fs_in.frag_pos);
    vec3 H = normalize(V + L);

    float dist = length(light_pos - fs_in.frag_pos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = light_color * attenuation;

    // Fresnel
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnel_schlick(N, V, F0);

    // Normal distribution function
    float D = normal_distribution_trowbridge_reitz_ggx(N, H, roughness);
    
    // Geometry
    float G = geometry_smith(N, V, L, roughness);

    // Cook-Torrance BRDF
    vec3 nominator = D * F * G;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);        
    vec3 Lo = (kD * albedo / PI + kS * specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    // frag_color = vec4(Lo, 1.0);
    frag_color = vec4(color, 1.0);
}
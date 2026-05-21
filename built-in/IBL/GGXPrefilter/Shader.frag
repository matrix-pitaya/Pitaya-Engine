#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform samplerCube uEnvCubemap;
layout(location = 0) uniform int uFace;
layout(location = 1) uniform float uRoughness;

in V2F
{
    vec2 texCoord;
} v2f;

const float PI = 3.14159265359;
const uint SAMPLE_COUNT = 1024;

vec3 UVToDirection(vec2 uv, int face)
{
    vec3 dir;
    switch (face)
    {
        case 0: dir = vec3( 1.0, -uv.y, -uv.x); break;
        case 1: dir = vec3(-1.0, -uv.y,  uv.x); break;
        case 2: dir = vec3( uv.x,  1.0,  uv.y); break;
        case 3: dir = vec3( uv.x, -1.0, -uv.y); break;
        case 4: dir = vec3( uv.x, -uv.y,  1.0); break;
        case 5: dir = vec3(-uv.x, -uv.y, -1.0); break;
    }
    return normalize(dir);
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16) | (bits >> 16);
    bits = ((bits & 0x55555555) << 1) | ((bits & 0xAAAAAAAA) >> 1);
    bits = ((bits & 0x33333333) << 2) | ((bits & 0xCCCCCCCC) >> 2);
    bits = ((bits & 0x0F0F0F0F) << 4) | ((bits & 0xF0F0F0F0) >> 4);
    bits = ((bits & 0x00FF00FF) << 8) | ((bits & 0xFF00FF00) >> 8);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{
    vec2 uv = v2f.texCoord * 2.0 - 1.0;
    vec3 N = UVToDirection(uv, uFace);
    vec3 R = N;
    vec3 V = R;

    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, uRoughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            prefilteredColor += textureLod(uEnvCubemap, L, 0.0).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / max(totalWeight, 0.001);
    FragColor = vec4(prefilteredColor, 1.0);
}

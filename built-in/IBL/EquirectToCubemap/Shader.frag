#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D uEquirect;
layout(location = 0) uniform int uFace;

in V2F
{
    vec2 texCoord;
} v2f;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 DirectionToUV(vec3 dir)
{
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec3 UVToDirection(vec2 uv, int face)
{
    vec3 dir;
    switch (face)
    {
        case 0: dir = vec3( 1.0, -uv.y, -uv.x); break; // +X
        case 1: dir = vec3(-1.0, -uv.y,  uv.x); break; // -X
        case 2: dir = vec3( uv.x,  1.0,  uv.y); break; // +Y
        case 3: dir = vec3( uv.x, -1.0, -uv.y); break; // -Y
        case 4: dir = vec3( uv.x, -uv.y,  1.0); break; // +Z
        case 5: dir = vec3(-uv.x, -uv.y, -1.0); break; // -Z
    }
    return normalize(dir);
}

void main()
{
    vec2 uv = v2f.texCoord * 2.0 - 1.0;
    vec3 dir = UVToDirection(uv, uFace);
    vec2 equirectUV = DirectionToUV(dir);
    vec3 color = texture(uEquirect, equirectUV).rgb;
    // HDR 天空盒中太阳像素能量极高，单样本命中会污染 irradiance/prefilter 整桶
    // clamp 到 500 保留 HDR 高动态但截断 firefly
    color = min(color, vec3(500.0));
    FragColor = vec4(color, 1.0);
}

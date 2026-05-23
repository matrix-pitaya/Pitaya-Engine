#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform samplerCube uEnvCubemap;
layout(location = 0) uniform int uFace;

in V2F
{
    vec2 texCoord;
} v2f;

const float PI = 3.14159265359;

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

void main()
{
    vec2 uv = v2f.texCoord * 2.0 - 1.0;
    vec3 N = UVToDirection(uv, uFace);
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = cross(N, right);

    vec3 irradiance = vec3(0.0);
    const uint PHI_STEPS   = 256u;
    const uint THETA_STEPS = 96u;

    for (uint pi = 0u; pi < PHI_STEPS; ++pi)
    {
        float phi = 2.0 * PI * (float(pi) + 0.5) / float(PHI_STEPS);
        for (uint ti = 0u; ti < THETA_STEPS; ++ti)
        {
            float theta = 0.5 * PI * (float(ti) + 0.5) / float(THETA_STEPS);
            vec3 tangentDir = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleDir = tangentDir.x * right + tangentDir.y * up + tangentDir.z * N;
            irradiance += textureLod(uEnvCubemap, sampleDir, 0.0).rgb * cos(theta) * sin(theta);
        }
    }
    float dphi   = 2.0 * PI / float(PHI_STEPS);
    float dtheta = 0.5 * PI / float(THETA_STEPS);
    irradiance = irradiance * dphi * dtheta;
    FragColor = vec4(irradiance, 1.0);
}

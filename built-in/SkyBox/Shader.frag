#version 460 core

#extension GL_ARB_bindless_texture : enable

out vec4 FragColor;

in V2F
{
    vec3 worldPos;
} v2f;

layout(std140, binding = 2) uniform SceneInfo
{
    vec4 AmbientColor;
    uvec2 EnvCubemapHandle;
    uvec2 IrradianceHandle;
    uvec2 PrefilteredHandle;
    uvec2 BRDFLUTHandle;
    float DeltaTime;
};

void main()
{
    vec3 direction = normalize(v2f.worldPos);
    vec3 color = textureLod(samplerCube(EnvCubemapHandle), direction, 0.0).rgb;
    FragColor = vec4(color, 1.0);
}

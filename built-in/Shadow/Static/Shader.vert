#version 460 core

layout (location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

struct InstanceInfo
{
    mat4 Model;
    mat4 Normal;
	uvec4 Params;
};

layout(std430, binding = 0) readonly buffer InstanceInfoSSBO
{
    InstanceInfo InstanceInfos[];
};

void main()
{
    uint index = gl_BaseInstance + gl_InstanceID;
    gl_Position = ViewProjection * InstanceInfos[index].Model * vec4(aPos, 1.0);
}
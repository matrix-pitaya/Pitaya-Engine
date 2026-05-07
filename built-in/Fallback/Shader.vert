#version 460 core

layout (location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

struct InstanceTransformInfo
{
    mat4 Model;
    mat4 Normal;
};

layout(std430, binding = 0) readonly buffer InstanceTransform
{
    InstanceTransformInfo InstanceTransformInfos[];
};

void main()
{
    uint index = gl_BaseInstance + gl_InstanceID;
    gl_Position = ViewProjection * InstanceTransformInfos[index].Model * vec4(aPos,1.0f);
}

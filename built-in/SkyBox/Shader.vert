#version 460 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

out V2F
{
    vec3 worldPos;
} v2f;

void main()
{
    mat4 viewNoTrans = mat4(mat3(View));
    vec4 worldPos = viewNoTrans * vec4(aPos, 1.0);
    v2f.worldPos = aPos;
    gl_Position = (Projection * worldPos).xyww;
}

#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout(std140, binding = 0) uniform CameraSnapshot
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;
	vec4 Position;
};

layout(std430, binding = 0) readonly buffer InstanceModelTransform
{
    mat4 Models[];
};

out V2F
{
	vec2 texCoord;
} v2f;

void main()
{
	uint index = gl_BaseInstance + gl_InstanceID;
	gl_Position = ViewProjection * Models[index] * vec4(aPos,1.0f);
	v2f.texCoord = aTexCoord;
}
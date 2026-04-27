#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D Albedo;

in V2F
{
	vec2 texCoord;
} v2f;

void main()
{
	FragColor = texture(Albedo,v2f.texCoord);
}

#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D MainTex;

in V2F
{
	vec2 texCoord;
} v2f;

void main()
{
    FragColor = texture(MainTex, v2f.texCoord);
}

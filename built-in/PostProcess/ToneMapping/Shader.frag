#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D MainTex;

layout(std140, binding = 1) uniform ToneMapping
{
    float Exposure;
};

in V2F
{
	vec2 texCoord;
} v2f;

vec3 ACESFitted(vec3 color)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(MainTex, v2f.texCoord).rgb;
    color *= Exposure;
    color = ACESFitted(color);
    FragColor = vec4(color, 1.0);
}

#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D MainTex;

layout(std140, binding = 1) uniform GammaCorrection 
{
    float Gamma;
};

in V2F
{
	vec2 texCoord;
} v2f;


void main()
{
    vec3 color = texture(MainTex, v2f.texCoord).rgb;
    color = pow(color, vec3(1.0 / Gamma));
    FragColor = vec4(color, 1.0);
}

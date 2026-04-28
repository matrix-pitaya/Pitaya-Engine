#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D Albedo;

layout(std140, binding = 0) uniform CameraSnapshot 
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;
	vec4 Position;       
};

struct LightInfo
{
    vec4 Position_Type;   // xyz=Pos, w=Type(0=Dir,1=Point,2=Spot)
    vec4 Direction;       // xyz=Dir
    vec4 Color_Intensity; // xyz=Col, w=Intensity
    vec4 Params;          // x=Radius, y=InnerCos, z=OuterCos      
};

layout(std430, binding = 2) readonly buffer LightBuffer 
{
    uint ActiveLightCount;   // C++ 传进来的 Header 长度
    uint pad0, pad1, pad2;   // 16 字节对齐黑洞
    LightInfo SceneLights[]; 
};

in V2F
{
	vec2 texCoord;
	vec3 fragPos;
	vec3 normal;
} v2f;

void main()
{
	vec4 albedoColor = texture(Albedo, v2f.texCoord);
	
	// Alpha 剔除保护
	if(albedoColor.a < 0.05) { discard; }

	vec3 norm = normalize(v2f.normal);
	vec3 viewDir = normalize(Position.xyz - v2f.fragPos);
	
	// 环境光基底
	vec3 finalColor = albedoColor.rgb * 0.1; 

	for(uint i = 0u; i < ActiveLightCount; ++i)
	{
		LightInfo light = SceneLights[i];
		vec3 lightColor = light.Color_Intensity.rgb * light.Color_Intensity.w;
		uint lightType = uint(light.Position_Type.w);
		
		vec3 lightDir;
		float attenuation = 1.0;
		float diff = 0.0;

		if(lightType == 0u) // 平行光
		{
			// Position_Type 存的是光源方向
			lightDir = normalize(-light.Direction.xyz); 
			diff = max(dot(norm, lightDir), 0.0);
		}
		else if(lightType == 1u) // 点光源
		{
			// Position_Type 存的是光源世界坐标
			lightDir = normalize(light.Position_Type.xyz - v2f.fragPos);
			diff = max(dot(norm, lightDir), 0.0);
			
			float distance = length(light.Position_Type.xyz - v2f.fragPos);
			float radius = max(light.Params.x, 0.001); // 防除0硬核保护
			
			// 线性滑梯衰减
			attenuation = clamp(1.0 - (distance / radius), 0.0, 1.0); 
			attenuation *= attenuation; 
		}
		else if (lightType == 2u) //聚光灯
		{
			lightDir = normalize(light.Position_Type.xyz - v2f.fragPos);
			diff = max(dot(norm, lightDir), 0.0);
			
			// 计算距离衰减
			float distance = length(light.Position_Type.xyz - v2f.fragPos);
			float radius = max(light.Params.x, 0.001);
			float distAttenuation = clamp(1.0 - (distance / radius), 0.0, 1.0);
			distAttenuation *= distAttenuation;
			
			// 计算圆锥角边缘平滑衰减
			float theta = dot(lightDir, normalize(-light.Direction.xyz)); 
			float innerCos = light.Params.y;
			float outerCos = light.Params.z;
			
			// epsilon为内外角的余弦差 用于平滑插值
			float epsilon = innerCos - outerCos;
			float intensity = clamp((theta - outerCos) / epsilon, 0.0, 1.0);
			
			// 最终衰减 = 距离衰减 * 边缘角度衰减
			attenuation = distAttenuation * intensity;
		}
		
		finalColor += albedoColor.rgb * diff * lightColor * attenuation;
	}
	
	FragColor = vec4(finalColor, albedoColor.a);
}

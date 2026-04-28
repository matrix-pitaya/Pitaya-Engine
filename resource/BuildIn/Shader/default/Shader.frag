#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D Albedo;
layout(binding = 1) uniform sampler2D Specular;

layout(std140, binding = 0) uniform CameraSnapshot 
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;
	vec4 Position;       
};

struct LightInfo
{
    vec4 Position_Type;
    vec4 Direction;
    vec4 Color_Intensity;
    vec4 Params;
};

layout(std430, binding = 2) readonly buffer LightBuffer 
{
    uint ActiveLightCount;
    uint pad0, pad1, pad2;
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
	if(albedoColor.a < 0.05) { discard; }

	vec3 norm = normalize(v2f.normal);
	vec3 viewDir = normalize(Position.xyz - v2f.fragPos);

	// 半球顶环境光(Hemisphere Ambient) 模拟天光与地光 使背面不再是一坨死板的单色 而是有色彩倾向的渐变
	vec3 skyColor = vec3(0.2, 0.25, 0.3);      // 模拟天空，微偏蓝
	vec3 groundColor = vec3(0.05, 0.04, 0.03); // 模拟地面反射，偏深褐
	
	// 根据法线的 Y 分量 朝上还是朝下 进行插值
	float hemiLevel = norm.y * 0.5 + 0.5;	// norm.y 在 [-1, 1] 之间，我们把它映射到 [0, 1]
	vec3 ambient = mix(groundColor, skyColor, hemiLevel) * 0.5; // 总强度系数 0.5
	
	vec3 finalColor = albedoColor.rgb * ambient; 
	vec3 specMapColor = texture(Specular, v2f.texCoord).rgb;

	for(uint i = 0u; i < ActiveLightCount; ++i)
	{
		LightInfo light = SceneLights[i];
		vec3 lightColor = light.Color_Intensity.rgb * light.Color_Intensity.w;
		uint lightType = uint(light.Position_Type.w);
		
		vec3 lightDir;
		float attenuation = 1.0;

		if(lightType == 0u) 
		{
			lightDir = normalize(-light.Direction.xyz); 
		}
		else if(lightType == 1u) 
		{
			lightDir = normalize(light.Position_Type.xyz - v2f.fragPos);
			float d = length(light.Position_Type.xyz - v2f.fragPos);
			attenuation = pow(clamp(1.0 - (d/max(light.Params.x, 0.01)), 0.0, 1.0), 2.0);
		} 
		else if(lightType == 2u) 
		{
			lightDir = normalize(light.Position_Type.xyz - v2f.fragPos);
			float d = length(light.Position_Type.xyz - v2f.fragPos);
			float dAtten = pow(clamp(1.0 - (d/max(light.Params.x, 0.01)), 0.0, 1.0), 2.0);
			float theta = dot(lightDir, normalize(-light.Direction.xyz));
			float sAtten = clamp((theta - light.Params.z) / (light.Params.y - light.Params.z), 0.0, 1.0);
			attenuation = dAtten * sAtten;
		}

		// 半兰伯特漫反射 (Half-Lambert Diffuse)
		float NdotL = dot(norm, lightDir);
		float halfLambert = NdotL * 0.5 + 0.5; 
		
		// 再叠一个平方 让暗部稍微深一点 过渡更丝滑
		float diff = halfLambert * halfLambert; 
		vec3 diffuse = albedoColor.rgb * diff;

		// 高光保持 Blinn-Phong
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0); 
		vec3 specular = specMapColor * spec; 

		finalColor += (diffuse + specular) * lightColor * attenuation;
	}
	
	FragColor = vec4(finalColor, albedoColor.a);
}
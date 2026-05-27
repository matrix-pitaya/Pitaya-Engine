#version 460 core

#extension GL_ARB_bindless_texture : enable

out vec4 FragColor;

layout(std430, binding = 4) readonly buffer MaterialSSBO { float Data[]; };

layout(binding = 9)  uniform sampler2DArrayShadow ShadowMap_CSM;
layout(binding = 10) uniform sampler2DArrayShadow ShadowMap_Spot;
layout(binding = 11) uniform sampler2DArrayShadow ShadowMap_Point;

struct ShadowSliceInfo
{
    uint MatrixOffset;
    uint LayerOffset;
    uint LightType;
    uint _pad0;
};

layout(std430, binding = 3) readonly buffer ShadowInfoBuffer
{
    uint DirLightCount;
    uint SpotLightCount;
    uint PointLightCount;
    uint TotalMatrixCount;
    uint CascadeSplitCount;
    uint ShadowSliceCount;
    uint _pad0, _pad1;
    vec4 ShadowPayload[];
};

layout(std140, binding = 0) uniform CameraSnapshot
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
    vec4 Position;
};

layout(std140, binding = 2) uniform SceneInfo
{
    vec4 AmbientColor;
    uvec2 EnvCubemapHandle;
    uvec2 IrradianceHandle;
    uvec2 PrefilteredHandle;
    uvec2 BRDFLUTHandle;
    float DeltaTime;
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
    uint _pad0_, _pad1_, _pad2_;
    LightInfo SceneLights[];
};

in V2F
{
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
    vec4 tangent;
    flat uint receiveShadow;
    flat uint materialByteOffset;
} v2f;

const float PI = 3.14159265359;

vec4 GetCascadeSplits(uint idx) { return ShadowPayload[idx]; }

mat4 GetShadowMatrix(uint idx)
{
    uint base = CascadeSplitCount + ShadowSliceCount + idx * 4u;
    return mat4(ShadowPayload[base], ShadowPayload[base+1u],
                ShadowPayload[base+2u], ShadowPayload[base+3u]);
}

ShadowSliceInfo GetShadowSlice(uint si)
{
    vec4 raw = ShadowPayload[CascadeSplitCount + si];
    ShadowSliceInfo s;
    s.MatrixOffset = floatBitsToUint(raw.x);
    s.LayerOffset = floatBitsToUint(raw.y);
    s.LightType = floatBitsToUint(raw.z);
    s._pad0 = floatBitsToUint(raw.w);
    return s;
}

float SampleShadowPCF(sampler2DArrayShadow sm, vec3 proj, float layer, float bias)
{
    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 1.0;

    vec2 ts = 1.0 / vec2(textureSize(sm, 0).xy);
    float s = 0.0;
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
            s += texture(sm, vec4(proj.xy + vec2(x,y)*ts, layer, proj.z - bias));
    return s / 9.0;
}

float CalcSpotShadow(vec3 wp, uint si)
{
    ShadowSliceInfo slice = GetShadowSlice(si);
    mat4 svp = GetShadowMatrix(slice.MatrixOffset);
    vec4 sc = svp * vec4(wp, 1.0);
    vec3 pc = sc.xyz / sc.w * 0.5 + 0.5;
    return SampleShadowPCF(ShadowMap_Spot, pc, float(slice.LayerOffset), 0.002);
}

float CalcPointShadow(vec3 wp, vec3 lp, uint si)
{
    vec3 d = wp - lp; vec3 a = abs(d);
    float m = max(a.x, max(a.y, a.z));
    uint f;
    if (m == a.x) { f = d.x > 0.0 ? 0u : 1u; }
    else if (m == a.y) { f = d.y > 0.0 ? 2u : 3u; }
    else { f = d.z > 0.0 ? 4u : 5u; }

    ShadowSliceInfo slice = GetShadowSlice(si);
    mat4 svp = GetShadowMatrix(slice.MatrixOffset + f);
    vec4 sc = svp * vec4(wp, 1.0);
    vec3 pc = sc.xyz / sc.w * 0.5 + 0.5;
    return SampleShadowPCF(ShadowMap_Point, pc, float(slice.LayerOffset + f), 0.005);
}

float CalcCSMShadow(vec3 wp, uint si)
{
    float vd = -(View * vec4(wp, 1.0)).z;
    uint csmIdx = si - SpotLightCount - PointLightCount;
    vec4 sp = GetCascadeSplits(csmIdx);
    uint ci = 3u;
    if (vd < sp.x) { ci = 0u; }
    else if (vd < sp.y) { ci = 1u; }
    else if (vd < sp.z) { ci = 2u; }

    ShadowSliceInfo slice = GetShadowSlice(si);
    mat4 svp = GetShadowMatrix(slice.MatrixOffset + ci);
    vec4 sc = svp * vec4(wp, 1.0);
    vec3 pc = sc.xyz / sc.w * 0.5 + 0.5;
    return SampleShadowPCF(ShadowMap_CSM, pc, float(slice.LayerOffset + ci), 0.001 * float(ci + 1u));
}

// PBR BRDF functions
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

void main()
{
    uint matBase = v2f.materialByteOffset / 4u;

    // Vector params
    vec4 baseColorFactor;
    baseColorFactor.x = Data[matBase + 0u];
    baseColorFactor.y = Data[matBase + 1u];
    baseColorFactor.z = Data[matBase + 2u];
    baseColorFactor.w = Data[matBase + 3u];

    vec4 emissiveFactor;
    emissiveFactor.x = Data[matBase + 4u];
    emissiveFactor.y = Data[matBase + 5u];
    emissiveFactor.z = Data[matBase + 6u];
    emissiveFactor.w = Data[matBase + 7u];

    // Float params
    float metallicFactor    = Data[matBase + 8u];
    float roughnessFactor   = Data[matBase + 9u];
    float occlusionStrength = Data[matBase + 10u];
    float alphaCutoff       = Data[matBase + 11u];

    // Texture handles (uint64 stored as 2 floats)
    #define GET_UVEC2(off) uvec2(floatBitsToUint(Data[matBase + off]), floatBitsToUint(Data[matBase + off + 1u]))
    uvec2 albedoHandle    = GET_UVEC2(12u);
    uvec2 normalHandle    = GET_UVEC2(14u);
    uvec2 metallicHandle  = GET_UVEC2(16u);
    uvec2 roughnessHandle = GET_UVEC2(18u);
    uvec2 aoHandle        = GET_UVEC2(20u);
    uvec2 emissiveHandle  = GET_UVEC2(22u);
    #undef GET_UVEC2

    vec4 albedo = texture(sampler2D(albedoHandle), v2f.texCoord);
    albedo *= baseColorFactor;
    if (albedo.a < alphaCutoff) { discard; }

    // TBN
    vec3 N = normalize(v2f.normal);
    vec3 T = normalize(v2f.tangent.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * v2f.tangent.w;
    mat3 TBN = mat3(T, B, N);

    vec3 normalTex = texture(sampler2D(normalHandle), v2f.texCoord).rgb;
    N = normalize(TBN * (normalTex * 2.0 - 1.0));

    vec3 metallicMap = texture(sampler2D(metallicHandle), v2f.texCoord).rgb;
    vec3 roughnessMap = texture(sampler2D(roughnessHandle), v2f.texCoord).rgb;
    float aoMap = texture(sampler2D(aoHandle), v2f.texCoord).r;
    vec3 emissiveMap = texture(sampler2D(emissiveHandle), v2f.texCoord).rgb;

    float metallic  = metallicFactor * metallicMap.r;
    float roughness = max(roughnessFactor * roughnessMap.r, 0.001);
    float ao = pow(aoMap, occlusionStrength);
    vec3 emissive = emissiveFactor.rgb * emissiveMap.rgb;

    vec3 V = normalize(Position.xyz - v2f.fragPos);
    vec3 albedoLinear = albedo.rgb;

    // Ambient (IBL or fallback AmbientColor)
    vec3 F0 = mix(vec3(0.04), albedoLinear, metallic);
    vec3 kS = FresnelSchlick(F0, max(dot(N, V), 0.0));
    vec3 kD_ibl = (1.0 - kS) * (1.0 - metallic);
    vec3 ambient;

    vec3 fallbackAmbient = AmbientColor.rgb * albedoLinear * ao;

    if (IrradianceHandle == uvec2(0))
    {
        ambient = fallbackAmbient;
    }
    else
    {
        vec3 irradiance = texture(samplerCube(IrradianceHandle), N).rgb;
        vec3 diffuseIBL = kD_ibl * albedoLinear * irradiance / PI;

        vec3 R = reflect(-V, N);
        const float MAX_MIP = 5.0;
        float lod = roughness * MAX_MIP;
        vec3 prefilteredColor = textureLod(samplerCube(PrefilteredHandle), R, lod).rgb;
        vec2 brdf = texture(sampler2D(BRDFLUTHandle), vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specularIBL = prefilteredColor * (kS * brdf.x + brdf.y);

        ambient = (diffuseIBL + specularIBL) * ao;
    }

    vec3 Lo = vec3(0.0);

    for (uint i = 0u; i < ActiveLightCount; ++i)
    {
        LightInfo light = SceneLights[i];
        vec3 lightColor = light.Color_Intensity.rgb * light.Color_Intensity.w;
        uint lightType = uint(light.Position_Type.w);

        vec3 L = vec3(0.0, 1.0, 0.0);
        float attenuation = 1.0;

        if (lightType == 0u) // Directional
        {
            L = normalize(-light.Direction.xyz);
        }
        else if (lightType == 1u) // Point
        {
            vec3 toLight = light.Position_Type.xyz - v2f.fragPos;
            L = normalize(toLight);
            float d = length(toLight);
            attenuation = pow(clamp(1.0 - (d / max(light.Params.x, 0.01)), 0.0, 1.0), 2.0);
        }
        else if (lightType == 2u) // Spot
        {
            vec3 toLight = light.Position_Type.xyz - v2f.fragPos;
            L = normalize(toLight);
            float d = length(toLight);
            float dAtten = pow(clamp(1.0 - (d / max(light.Params.x, 0.01)), 0.0, 1.0), 2.0);
            float theta = dot(L, normalize(-light.Direction.xyz));
            float sAtten = clamp((theta - light.Params.z) / (light.Params.y - light.Params.z), 0.0, 1.0);
            attenuation = dAtten * sAtten;
        }

        vec3 H = normalize(V + L);

        vec3 radiance = lightColor * attenuation;
        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float NdotH = max(dot(N, H), 0.0);

        // Cook-Torrance
        vec3 F0 = mix(vec3(0.04), albedoLinear, metallic);
        vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 specular = (D * G * F) / (4.0 * NdotL * NdotV + 0.0001);
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedoLinear / PI;

        // Shadow
        float shadow = 1.0;
        if (bool(v2f.receiveShadow & 1u))
        {
            float sliceIdx = light.Params.w;
            if (sliceIdx >= 0.0)
            {
                uint si = uint(sliceIdx);
                if (lightType == 0u) { shadow = CalcCSMShadow(v2f.fragPos, si); }
                else if (lightType == 2u) { shadow = CalcSpotShadow(v2f.fragPos, si); }
                else if (lightType == 1u) { shadow = CalcPointShadow(v2f.fragPos, light.Position_Type.xyz, si); }
            }
        }

        Lo += (diffuse + specular) * radiance * NdotL * shadow;
    }

    vec3 finalColor = ambient + Lo + emissive;
    FragColor = vec4(finalColor, albedo.a);
}

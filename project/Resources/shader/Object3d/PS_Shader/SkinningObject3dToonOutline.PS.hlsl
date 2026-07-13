#include "../Object3d.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float3 padding;
    float4x4 uvTransform;
    float shininess;
    float environmentCoefficient;
    int grayscaleEnabled;
    int sepiaEnabled;
    float distortionStrength;
    float distortionFalloff;
    float2 padding2;
    float4 outlineColor;
    float outlineWidth;
    int dissolveEnabled;
    float dissolveThreshold;
    float dissolveEdgeWidth;
};
struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    int shadowEnabled;
    float3 padding;
};
struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
    int shadowEnabled;
    float padding;
};
struct PointLightCount
{
    uint count;
    float3 padding;
};
struct SpotLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
    int shadowEnabled;
    float padding;
};
struct SpotLightCount
{
    uint count;
    float3 padding;
};
struct AreaLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 normal;
    float width;
    float height;
    float radius;
    float decay;
    int shadowEnabled;
    float3 padding;
};
struct AreaLightCount
{
    uint count;
    float3 padding;
};
struct Camera
{
    float3 worldPosition;
    float padding;
    float2 screenSize;
    int fullscreenGrayscaleEnabled;
    int fullscreenSepiaEnabled;
    float2 padding2;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b3);
ConstantBuffer<Camera> gCamera : register(b4);
ConstantBuffer<PointLightCount> gPointLightCount : register(b5);
ConstantBuffer<SpotLightCount> gSpotLightCount : register(b6);
ConstantBuffer<AreaLightCount> gAreaLightCount : register(b7);
StructuredBuffer<SpotLight> gSpotLights : register(t2);
StructuredBuffer<PointLight> gPointLights : register(t1);
StructuredBuffer<AreaLight> gAreaLights : register(t3);
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t4);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};
float ComputeDissolveMask(float3 worldPosition, float2 texcoord)
{
    float3 seed = worldPosition * 3.1f + float3(texcoord, texcoord.x + texcoord.y) * 17.0f;
    return frac(sin(dot(seed, float3(12.9898f, 78.233f, 37.719f))) * 43758.5453f);
}

void ApplyDissolve(float3 worldPosition, float2 texcoord)
{
    if (gMaterial.dissolveEnabled == 0)
    {
        return;
    }

    float mask = ComputeDissolveMask(worldPosition, texcoord);
    if (mask <= gMaterial.dissolveThreshold)
    {
        discard;
    }
}
static const float kToonShadowThreshold = 0.5f;
static const float kToonShadowSoftness = 0.04f;
static const float kToonShadowStrength = 0.35f;
static const float kToonLightIntensityMax = 1.00f;

float ComputeToonShadowMask(float NdotL)
{
    float saturatedNdotL = saturate(NdotL);
    return smoothstep(
        kToonShadowThreshold - kToonShadowSoftness,
        kToonShadowThreshold + kToonShadowSoftness,
        saturatedNdotL);
}

PixelShaderOutput main(Object3dVertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    if (textureColor.a < 0.1f)
    {
        discard;
    }
    ApplyDissolve(input.worldPosition, transformedUV.xy);
    float3 finalColor = gMaterial.color.rgb * textureColor.rgb;

    if (gMaterial.enableLighting != 0)
    {
        float3 N = normalize(input.normal);
        float lightAccumulation = 0.0f;

        float3 directionalLightVector = -normalize(gDirectionalLight.direction);
        float directionalNdotL = dot(N, directionalLightVector);
        lightAccumulation += ComputeToonShadowMask(directionalNdotL) * gDirectionalLight.intensity;

        for (uint index = 0; index < gPointLightCount.count; ++index)
        {
            PointLight pointLight = gPointLights[index];
            float3 lightVector = pointLight.position - input.worldPosition;
            float distanceToLight = length(lightVector);
            float3 lightDirection = normalize(lightVector);
            float attenuation = pow(saturate(1.0f - distanceToLight / pointLight.radius), pointLight.decay);
            float NdotL = dot(N, lightDirection);
            lightAccumulation += ComputeToonShadowMask(NdotL) * pointLight.intensity * attenuation;
        }

        for (uint index = 0; index < gSpotLightCount.count; ++index)
        {
            SpotLight spotLight = gSpotLights[index];
            float3 lightVector = spotLight.position - input.worldPosition;
            float distanceToLight = length(lightVector);
            float3 lightDirection = normalize(lightVector);
            float attenuation = pow(saturate(1.0f - distanceToLight / spotLight.distance), spotLight.decay);
            float3 spotDirection = normalize(spotLight.direction);
            float cone = dot(-lightDirection, spotDirection);
            float falloff = saturate((cone - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
            float NdotL = dot(N, lightDirection);
            lightAccumulation += ComputeToonShadowMask(NdotL) * spotLight.intensity * attenuation * falloff;
        }

        for (uint index = 0; index < gAreaLightCount.count; ++index)
        {
            AreaLight areaLight = gAreaLights[index];
            float3 lightVector = areaLight.position - input.worldPosition;
            float distanceToLight = length(lightVector);
            float3 lightDirection = normalize(lightVector);
            float attenuation = pow(saturate(1.0f - distanceToLight / areaLight.radius), areaLight.decay);
            float facing = saturate(dot(normalize(areaLight.normal), -lightDirection));
            float areaScale = areaLight.width * areaLight.height;
            float NdotL = dot(N, lightDirection);
            lightAccumulation += ComputeToonShadowMask(NdotL) * areaLight.intensity * attenuation * facing * areaScale;
        }

        lightAccumulation = min(lightAccumulation, kToonLightIntensityMax);
        float toonBand = step(0.5f, lightAccumulation);
        float shadeIntensity = lerp(kToonShadowStrength, 1.0f, toonBand);

        float3 viewDirection = normalize(input.worldPosition - gCamera.worldPosition);
        float3 reflectedDirection = reflect(viewDirection, N);
        float3 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedDirection).rgb;

        float3 litColor = finalColor * shadeIntensity;
        litColor += environmentColor * gMaterial.environmentCoefficient;
        finalColor = litColor;
    }

    float3 viewDirection = normalize(gCamera.worldPosition - input.worldPosition);
    float viewDot = abs(dot(normalize(input.normal), viewDirection));
    float edgeMask = saturate((1.0f - viewDot - 0.2f) / 0.3f);

    output.color.rgb = lerp(finalColor, gMaterial.outlineColor.rgb, edgeMask * gMaterial.outlineColor.a);
    output.color.a = gMaterial.color.a * textureColor.a;
    return output;
}
#include "Skybox.hlsli"
#include "../Object3d/Object3d.hlsli"

TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_Target0;
};

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
    float3 outlinePadding;
};
ConstantBuffer<Material> gMaterial : register(b0);
float3 ApplySkyboxToneMap(float3 color)
{
   
    const float maxChannel = max(max(color.r, color.g), color.b);
    if (maxChannel <= 1.0f)
    {
        return color;
    }

    const float exposure = 0.35f;
    color *= exposure;
    
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}

PixelShaderOutput main(SkyboxVertexShaderOutput input)
{
    PixelShaderOutput output;
    float3 sampleDirection = normalize(input.texcoord);
    float4 textureColor = gTexture.Sample(gSampler, sampleDirection);
    output.color = textureColor * gMaterial.color;
    output.color.rgb = ApplySkyboxToneMap(output.color.rgb);
    return output;
}
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
PixelShaderOutput main(SkyboxVertexShaderOutput input)
{
    PixelShaderOutput output;
    float3 sampleDirection = normalize(input.texcoord);
    float4 textureColor = gTexture.SampleLevel(gSampler, sampleDirection, 0.0f);
    output.color = textureColor * gMaterial.color;
    output.color.rgb = ApplyEnvironmentMapToneMap(output.color.rgb);
    return output;
}
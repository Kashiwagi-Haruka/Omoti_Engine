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
    float4 outlineColor;
    float outlineWidth;
    int dissolveEnabled;
    float dissolveThreshold;
    float dissolveEdgeWidth;
    float4 dissolveEdgeColor;
};
ConstantBuffer<Material> gMaterial : register(b0);
struct Camera
{
    float3 worldPosition;
    float padding;
    float2 screenSize;
    int fullscreenGrayscaleEnabled;
    int fullscreenSepiaEnabled;
    float2 padding2;
};
ConstantBuffer<Camera> gCamera : register(b4);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};
float3 ApplyGrayscale(float3 color)
{
    if (gMaterial.grayscaleEnabled == 0 && gCamera.fullscreenGrayscaleEnabled == 0)
    {
        return color;
    }
    float y = dot(color, float3(0.2125f, 0.7154f, 0.0721f));
    return float3(y, y, y);
}
float3 ApplySepia(float3 color)
{
    if (gMaterial.sepiaEnabled == 0 && gCamera.fullscreenSepiaEnabled == 0)
    {
        return color;
    }

    float3 sepia;
    sepia.r = dot(color, float3(0.393f, 0.769f, 0.189f));
    sepia.g = dot(color, float3(0.349f, 0.686f, 0.168f));
    sepia.b = dot(color, float3(0.272f, 0.534f, 0.131f));
    return saturate(sepia);
}
PixelShaderOutput main(Object3dVertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    float2 centeredUv = transformedUV.xy - float2(0.5f, 0.5f);
    float distanceFromCenter = length(centeredUv) * 2.0f;
    float wideHaze = exp(-distanceFromCenter * distanceFromCenter * 0.72f);
    float outerMist = exp(-distanceFromCenter * distanceFromCenter * 0.24f);


    float haze = wideHaze * 0.62f + outerMist * 0.38f;

    float centerSuppression = lerp(0.24f, 1.0f, smoothstep(0.0f, 0.62f, distanceFromCenter));
    float3 glowColor = pow(saturate(gMaterial.color.rgb), 3.0f);
    float emission = haze * centerSuppression * 0.105f;

    output.color.rgb = glowColor * emission;
    output.color.a = textureColor.a * gMaterial.color.a * haze * 0.58f;
    output.color.rgb = ApplyGrayscale(output.color.rgb);
    output.color.rgb = ApplySepia(output.color.rgb);
    if (output.color.a < 0.002f)
    {
        discard;
    }

    return output;
}
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

    float3 baseColor = textureColor.rgb * gMaterial.color.rgb;
    float luminance = dot(baseColor, float3(0.2126f, 0.7152f, 0.0722f));
    float glowStrength = 1.0f + saturate(gMaterial.environmentCoefficient) * 2.0f;

    // UV中心を白熱させ、外周へ滑らかに減衰させる。発光RTに均一な板を
    // 書くのではなく明確な輝度勾配を作ることで、ブルームが中心から広がる。
    float2 centeredUv = transformedUV.xy - float2(0.5f, 0.5f);
    float distanceFromCenter = length(centeredUv) * 2.0f;
    float core = 1.0f - smoothstep(0.0f, 0.42f, distanceFromCenter);
    float body = 1.0f - smoothstep(0.18f, 1.0f, distanceFromCenter);
    float edgeFade = 1.0f - smoothstep(0.72f, 1.0f, distanceFromCenter);
    float brightness = 0.45f + body * 2.2f + core * 4.5f;
    float3 hotCoreColor = lerp(baseColor, float3(1.0f, 1.0f, 0.82f), core * 0.72f);

    output.color.rgb = hotCoreColor * brightness * glowStrength * (0.65f + luminance * 0.7f);
    output.color.a = textureColor.a * gMaterial.color.a * edgeFade;
    output.color.rgb = ApplyGrayscale(output.color.rgb);
    output.color.rgb = ApplySepia(output.color.rgb);
    if (textureColor.a < 0.5f)
    {
        discard;
    }
    if (textureColor.a == 0.0f)
    {
        discard;
    }
    if (output.color.a == 0.0f)
    {
        discard;
    }

    return output;
}
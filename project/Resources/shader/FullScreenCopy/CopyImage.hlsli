struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

cbuffer PostEffectParameters : register(b0)
{
    float vignetteStrength;
    float3 vignetteColor;
    float randomNoiseEnabled;
    float randomNoiseScale;
    float randomNoiseTime;
    float randomNoiseBlendMode;
    float boxFilterKernelSize;
    float fullscreenGrayscaleEnabled;
    float fullscreenSepiaEnabled;
    float fullscreenFilterType;
    float gaussianFilterSigma;
    float dissolveEnabled;
    float dissolveThreshold;
    float dissolveEdgeWidth;
    float dissolvePadding;
    float2 radialBlurCenter;
    float radialBlurWidth;
    float radialBlurSampleCount;
    float chromaticAberrationEnabled;
    float chromaticAberrationIntensity;
    float2 chromaticAberrationPadding;
    float selectiveBloomEnabled;
    float selectiveBloomIntensity;
    float selectiveBloomRadius;
    float selectiveBloomPadding;
};
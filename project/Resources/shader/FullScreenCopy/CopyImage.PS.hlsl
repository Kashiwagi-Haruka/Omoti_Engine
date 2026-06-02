#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gOutlineTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};
float rand2dTo1d(float2 value)
{
    const float2 smallValue = sin(value);
    const float2 dotDir = float2(12.9898f, 78.233f);
    float random = dot(smallValue, dotDir);
    return frac(sin(random) * 143758.5453f);
}
float3 ApplyGrayscale(float3 color)
{
    if (fullscreenGrayscaleEnabled < 0.5f)
    {
        return color;
    }
    float y = dot(color, float3(0.2125f, 0.7154f, 0.0721f));
    return float3(y, y, y);
}

float3 ApplySepia(float3 color)
{
    if (fullscreenSepiaEnabled < 0.5f)
    {
        return color;
    }

    float3 sepia;
    sepia.r = dot(color, float3(0.393f, 0.769f, 0.189f));
    sepia.g = dot(color, float3(0.349f, 0.686f, 0.168f));
    sepia.b = dot(color, float3(0.272f, 0.534f, 0.131f));
    return saturate(sepia);
}
float Gauss(float x, float y, float sigma)
{
    static const float PI = 3.14159265f;
    float safeSigma = max(sigma, 0.001f);
    float exponent = -((x * x + y * y) * rcp(2.0f * safeSigma * safeSigma));
    float denominator = 2.0f * PI * safeSigma * safeSigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    int kernelSize = max((int) (boxFilterKernelSize), 1);
    if (kernelSize > 1)
    {
        if ((kernelSize & 1) == 0)
        {
            kernelSize -= 1;
        }
        kernelSize = min(kernelSize, 15);
        const int radius = kernelSize / 2;

        uint width = 0;
        uint height = 0;
        gTexture.GetDimensions(width, height);
        float2 uvStepSize = float2(rcp((float) width), rcp((float) height));

        bool useGaussianFilter = fullscreenFilterType > 0.5f;
        float3 filteredColor = float3(0.0f, 0.0f, 0.0f);
        float weight = 0.0f;
        [loop]
        for (int y = -7; y <= 7; ++y)
        {
            if (abs(y) > radius)
            {
                continue;
            }
            [loop]
            for (int x = -7; x <= 7; ++x)
            {
                if (abs(x) > radius)
                {
                    continue;
                }
                float2 offset = float2((float) (x), (float) (y)) * uvStepSize;
                float2 texcoord = saturate(input.texcoord + offset);
                float kernelWeight = useGaussianFilter ? Gauss((float) x, (float) y, gaussianFilterSigma) : 1.0f;
                filteredColor += gTexture.Sample(gSampler, texcoord).rgb * kernelWeight;
                weight += kernelWeight;
            }
        }
        output.color.rgb = filteredColor / max(weight, 0.0001f);
    }
    
    float2 centeredUv = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = centeredUv.x * centeredUv.y * 16.0f;
    vignette = saturate(pow(vignette, 0.8f));
    float edgeMask = 1.0f - vignette;
    float vignetteBlend = saturate(edgeMask * vignetteStrength);
    output.color.rgb = lerp(output.color.rgb, vignetteColor, vignetteBlend);

    if (randomNoiseEnabled > 0.5f)
    {
        float random = rand2dTo1d(input.texcoord * randomNoiseScale * randomNoiseTime);
        float3 noiseColor = float3(random, random, random);

        if (randomNoiseBlendMode < 0.5f)
        {
            output.color.rgb = noiseColor;
        }
        else if (randomNoiseBlendMode < 1.5f)
        {
            output.color.rgb += noiseColor;
        }
        else if (randomNoiseBlendMode < 2.5f)
        {
            output.color.rgb -= noiseColor;
        }
        else if (randomNoiseBlendMode < 3.5f)
        {
            output.color.rgb *= noiseColor;
        }
        else
        {
            output.color.rgb = 1.0f - ((1.0f - output.color.rgb) * (1.0f - noiseColor));
        }

        output.color.rgb = saturate(output.color.rgb);
    }
    output.color.rgb = ApplyGrayscale(output.color.rgb);
    output.color.rgb = ApplySepia(output.color.rgb);
    
    float4 outlineColor = gOutlineTexture.Sample(gSampler, input.texcoord);
    output.color.rgb = lerp(output.color.rgb, outlineColor.rgb, saturate(outlineColor.a));

    return output;
}
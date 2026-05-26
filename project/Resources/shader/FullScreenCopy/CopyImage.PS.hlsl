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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    int kernelSize = max(static_cast<int>(boxFilterKernelSize), 1);
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
        float2 uvStepSize = float2(rcp(width), rcp(height));

        float3 filteredColor = float3(0.0f, 0.0f, 0.0f);
        float sampleCount = 0.0f;
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
                float2 offset = float2(static_cast <
                float > (x), static_cast <
                float > (y)) * uvStepSize;
                float2 texcoord = saturate(input.texcoord + offset);
                filteredColor += gTexture.Sample(gSampler, texcoord).rgb;
                sampleCount += 1.0f;
            }
        }
        output.color.rgb = filteredColor / max(sampleCount, 1.0f);
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

    float4 outlineColor = gOutlineTexture.Sample(gSampler, input.texcoord);
    output.color.rgb = lerp(output.color.rgb, outlineColor.rgb, saturate(outlineColor.a));

    return output;
}
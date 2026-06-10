struct Object3dVertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    float4 directionalShadowPosition : TEXCOORD1;
    float4 pointShadowPosition : TEXCOORD2;
    float4 spotShadowPosition : TEXCOORD3;
    float4 areaShadowPosition : TEXCOORD4;
};

float3 ApplyEnvironmentMapToneMap(float3 color)
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
#ifndef OVERLAY_HLSLI
#define OVERLAY_HLSLI

// center : 画像を貼る中心。UV中央なら(0.5, 0.5)
// size   : UV空間での画像の大きさ。必ず正の値にする
float2 CalculateOverlayUV(float2 meshUV,float2 center,float2 size){
    return (meshUV - center) / max(size, float2(0.0001f, 0.0001f))
         + 0.5f;
}

// 画像がUV範囲外で繰り返されるのを防ぐ
float CalculateOverlayBounds(float2 uv){
    return (
        uv.x >= 0.0f && uv.x <= 1.0f &&
        uv.y >= 0.0f && uv.y <= 1.0f
    ) ? 1.0f : 0.0f;
}

float3 BlendOverlay(float3 baseColor,float4 overlayColor,float mask,float opacity){
    
    float weight = saturate(overlayColor.a * mask * opacity);

    return lerp(baseColor, overlayColor.rgb, weight);
}

#endif
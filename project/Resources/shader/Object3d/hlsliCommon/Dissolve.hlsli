#ifndef DISSOLVE_HLSLI
#define DISSOLVE_HLSLI

// amount : 0で非表示、1で全表示
// softness : 境界のぼかし幅。0ならくっきり
float CalculateDissolveMask(float noise,float amount,float softness){
    if (amount <= 0.0f)
        return 0.0f;

    if (amount >= 1.0f)
        return 1.0f;

    if (softness <= 0.0f)
        return step(noise, amount);

    // 境界をノイズ範囲の外まで移動させる
    float threshold = lerp(-softness, 1.0f + softness, amount);

    return 1.0f - smoothstep(
        threshold - softness,
        threshold + softness,
        noise
    );
}

#endif
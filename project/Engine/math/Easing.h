#pragma once

namespace Easing {

// 各関数の t は進行度（0.0f ～ 1.0f）
// 戻り値は補間に使用する係数
// Back は戻り値が一時的に 0 ～ 1 の範囲を超える

// 等速
float Linear(float t);

// 二次関数：加速・減速・加速減速
float EaseInQuad(float t);
float EaseOutQuad(float t);
float EaseInOutQuad(float t);

// 三次関数：加速・減速・加速減速
float EaseInCubic(float t);
float EaseOutCubic(float t);
float EaseInOutCubic(float t);

// サイン関数：なめらかな加速・減速・加速減速
float EaseInSine(float t);
float EaseOutSine(float t);
float EaseInOutSine(float t);

// 少し逆方向に動いてから進む・行き過ぎて戻る
float EaseInBack(float t);
float EaseOutBack(float t);
float EaseInOutBack(float t);

// 跳ねる動き
float EaseInBounce(float t);
float EaseOutBounce(float t);
float EaseInOutBounce(float t);

} // namespace Easing
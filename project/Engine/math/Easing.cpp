#include "Easing.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace {

// 円周率
constexpr float kPi = std::numbers::pi_v<float>;

// 進行度を 0 ～ 1 に制限する
float ClampProgress(float t) { return std::clamp(t, 0.0f, 1.0f); }

} // namespace

namespace Easing {

// 等速
float Linear(float t) { return ClampProgress(t); }

// 二次関数で加速する
float EaseInQuad(float t) {
	t = ClampProgress(t);
	return t * t;
}

// 二次関数で減速する
float EaseOutQuad(float t) {
	t = ClampProgress(t);
	const float u = 1.0f - t;
	return 1.0f - u * u;
}

// 前半は加速、後半は減速する
float EaseInOutQuad(float t) {
	t = ClampProgress(t);

	if (t < 0.5f) {
		return 2.0f * t * t;
	}

	const float u = 1.0f - t;
	return 1.0f - 2.0f * u * u;
}

// 三次関数で加速する
float EaseInCubic(float t) {
	t = ClampProgress(t);
	return t * t * t;
}

// 三次関数で減速する
float EaseOutCubic(float t) {
	t = ClampProgress(t);
	const float u = 1.0f - t;
	return 1.0f - u * u * u;
}

// 前半は加速、後半は減速する
float EaseInOutCubic(float t) {
	t = ClampProgress(t);

	if (t < 0.5f) {
		return 4.0f * t * t * t;
	}

	const float u = 1.0f - t;
	return 1.0f - 4.0f * u * u * u;
}

// サイン曲線でなめらかに加速する
float EaseInSine(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	return 1.0f - std::cos(t * kPi * 0.5f);
}

// サイン曲線でなめらかに減速する
float EaseOutSine(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	return std::sin(t * kPi * 0.5f);
}

// サイン曲線でなめらかに加速・減速する
float EaseInOutSine(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	return (1.0f - std::cos(kPi * t)) * 0.5f;
}

// 少し逆方向に動いてから加速する
float EaseInBack(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	return c3 * t * t * t - c1 * t * t;
}

// 終点を少し行き過ぎてから戻る
float EaseOutBack(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	const float u = t - 1.0f;
	return 1.0f + c3 * u * u * u + c1 * u * u;
}

// 開始時に少し戻り、終了時に少し行き過ぎる
float EaseInOutBack(float t) {
	t = ClampProgress(t);

	if (t == 0.0f || t == 1.0f) {
		return t;
	}

	constexpr float c1 = 1.70158f;
	constexpr float c2 = c1 * 1.525f;

	if (t < 0.5f) {
		const float u = 2.0f * t;
		return u * u * ((c2 + 1.0f) * u - c2) * 0.5f;
	}

	const float u = 2.0f * t - 2.0f;
	return (u * u * ((c2 + 1.0f) * u + c2) + 2.0f) * 0.5f;
}

// 開始時に跳ねてから進む
float EaseInBounce(float t) {
	t = ClampProgress(t);
	return 1.0f - EaseOutBounce(1.0f - t);
}

// 終了時に跳ねる
float EaseOutBounce(float t) {
	t = ClampProgress(t);

	constexpr float n1 = 7.5625f;
	constexpr float d1 = 2.75f;

	if (t < 1.0f / d1) {
		return n1 * t * t;
	}

	if (t < 2.0f / d1) {
		t -= 1.5f / d1;
		return n1 * t * t + 0.75f;
	}

	if (t < 2.5f / d1) {
		t -= 2.25f / d1;
		return n1 * t * t + 0.9375f;
	}

	t -= 2.625f / d1;
	return n1 * t * t + 0.984375f;
}

// 開始時と終了時に跳ねる
float EaseInOutBounce(float t) {
	t = ClampProgress(t);

	if (t < 0.5f) {
		return (1.0f - EaseOutBounce(1.0f - 2.0f * t)) * 0.5f;
	}

	return (1.0f + EaseOutBounce(2.0f * t - 1.0f)) * 0.5f;
}

} // namespace Easing
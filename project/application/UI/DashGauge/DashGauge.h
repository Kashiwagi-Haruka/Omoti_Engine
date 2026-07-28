#pragma once
#include "Vector2.h"
#include <array>
#include <memory>

class Sprite;

class DashGauge {
	static constexpr int kSegmentCount = 28;

	Vector2 position_{};
	Vector2 segmentSize_{};
	std::array<std::unique_ptr<Sprite>, kSegmentCount> frameSprites_{};
	std::array<std::unique_ptr<Sprite>, kSegmentCount> fillSprites_{};
	float gaugeRate_ = 1.0f;
	bool dashUIView_ = false;
	float alpha_ = 0.0f;

public:
	void Initialize();
	void Update();
	void Draw();
	void SetGaugeRate(float gaugeRate);
	void SetDamageUIView(bool damageUIView);
};
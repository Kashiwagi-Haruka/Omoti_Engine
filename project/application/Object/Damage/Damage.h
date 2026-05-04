#pragma once
#include "Primitive/Primitive.h"
#include "Transform.h"
#include <array>
#include <memory>
#include <vector>

class Camera;

class Damage {
	int MaxDamage_ = 99999999;

public:
	void Initialize(Camera* camera);
	void Update();
	void Draw();
	void SetDamageValue(int damage);
	void SetPosition(const Vector3& position);

private:
	static constexpr float kShowDuration_ = 3.0f;
	static constexpr float kFadeDuration_ = 0.5f;
	static constexpr int kDigitCount = 8;
	std::array<std::unique_ptr<Primitive>, kDigitCount> digitPrimitives_{};
	std::vector<int> digits_{};
	Transform transform_ = {
	    {0.4f, 0.4f, 0.4f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 2.0f, 0.0f},
	};
	Camera* camera_ = nullptr;
	bool isVisible_ = false;
	bool isFading_ = false;
	float timer_ = 0.0f;
	float alpha_ = 1.0f;
	float digitSpacing_ = 0.55f;
};
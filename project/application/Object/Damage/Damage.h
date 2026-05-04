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
	void SetVisible(bool visible) { isVisible_ = visible; }

private:
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
	float digitSpacing_ = 0.55f;
};
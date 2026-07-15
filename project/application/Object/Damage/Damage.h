#pragma once
#include "Object/Characters/Base/Attribute.h"
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
	void SetScale(const Vector3& scale);
	const Vector3& GetPosition() const { return transform_.translate; }
	bool IsVisible() const { return isVisible_; }
	void SetAttribute(Attribute attribute);
	void SetReactionAttribute(Attribute previousAttribute, Attribute appliedAttribute);
	void SetIsCritical(bool isCritical) { isCritical_ = isCritical; }

private:
	static constexpr float kShowDuration_ = 1.0f;
	static constexpr float kFadeDuration_ = 0.5f;
	static constexpr float kAppearDuration_ = 0.2f;
	static constexpr float kAppearStartAlpha_ = 0.5f;
	static constexpr float kAppearStartScale_ = 1.4f;
	static constexpr int kDigitCount = 8;
	std::array<std::unique_ptr<Primitive>, kDigitCount> digitPrimitives_{};
	std::vector<int> digits_{};
	Transform transform_ = {
	    {0.4f, 0.4f, 0.4f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 2.0f, 0.0f},
	};
	Vector3 basePosition_ = {0.0f, 2.0f, 0.0f};
	Vector3 damagePositionOffset_ = {0.0f, 0.0f, 0.0f};
	int damagePositionOffsetIndex_ = 0;
	Camera* camera_ = nullptr;
	Attribute attribute_ = Attribute::None;
	Attribute reactionPreviousAttribute_ = Attribute::None;
	bool isVisible_ = false;
	bool isFading_ = false;
	bool isCritical_ = false;
	float timer_ = 0.0f;
	float appearTimer_ = 0.0f;
	float alpha_ = 1.0f;
	float digitSpacing_ = 0.55f;
};
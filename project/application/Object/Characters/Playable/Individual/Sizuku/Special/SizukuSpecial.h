#pragma once
#include "Object/Characters/Base/Attribute.h"
#include "SizukuSpecialAttributeBase.h"
#include <memory>
class Camera;
class SizukuSpecial {
	bool isStarted_ = false, isEnd_ = false;
	float elapsedTime_ = 0, animationTime_ = 0, animationTimeMax_ = 5, sizukuHeight_ = 2;
	int damageId_ = 0, rainDamageId_ = 0;
	Attribute attribute_ = Attribute::Ice;
	std::unique_ptr<SizukuSpecialAttributeBase> fireSpecial_, iceSpecial_, windSpecial_, thunderSpecial_, imaginarySpecial_, quantumSpecial_;
	SizukuSpecialAttributeBase* activeSpecial_ = nullptr;
	Transform sizukuTransform_{};
	Camera* camera_ = nullptr;
	SizukuSpecialContext Context() const { return {sizukuTransform_, camera_, sizukuHeight_, elapsedTime_}; }

public:
	SizukuSpecial();
	void Initialize();
	void Update();
	void Draw(Camera* camera);
	void Start();
	void End();
	void SetCamera(Camera* c) { camera_ = c; }
	void SetSizukuTransform(Transform t) { sizukuTransform_ = t; }
	void SetSizukuHeight(float h) { sizukuHeight_ = h; }
	void SetAttribute(Attribute a) { attribute_ = a; }
	Attribute GetAttribute() const { return attribute_; }
	bool isEnd() const { return isEnd_; }
	bool IsAnimationFinished() const { return animationTime_ >= animationTimeMax_; }
	bool IsFlowerDamaging() const;
	bool IsRainDamaging() const;
	Vector3 GetDamagePosition() const { return activeSpecial_ ? activeSpecial_->GetDamagePosition() : Vector3{}; }
	Vector3 GetDamageScale() const { return activeSpecial_ ? activeSpecial_->GetDamageScale() : Vector3{}; }
	int GetDamageId() const { return damageId_; }
	const std::vector<Transform>& GetRainDamageTransforms() const { return activeSpecial_->GetRainTransforms(); }
	Vector3 GetRainDamageScale() const { return {.75f, 1.25f, .75f}; }
	int GetRainDamageId() const { return rainDamageId_; }
};
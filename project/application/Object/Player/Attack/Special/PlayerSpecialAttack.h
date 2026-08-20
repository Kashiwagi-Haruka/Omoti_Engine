#pragma once
#include "Object/Characters/Playable/Individual/Sizuku/SizukuSpecial.h"
#include "Object3d/Object3d.h"
#include "Transform.h"
#include <memory>
#include <vector>
class PlayerSpecialAttack {

	std::unique_ptr<SizukuSpecial> sizuku_;
	Camera* camera_ = nullptr;
	Transform transform_;

public:
	PlayerSpecialAttack();
	void Initialize();
	void Start();
	void End();
	void Update();
	void Draw();
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPlayerTransform(const Transform& playerTransform);
	bool IsEnd() { return sizuku_->isEnd(); };
	bool IsAnimationFinished() const { return sizuku_->IsAnimationFinished(); }
	bool IsDamaging() const { return sizuku_->IsFlowerDamaging(); }
	Vector3 GetDamagePosition() const { return sizuku_->GetDamagePosition(); }
	Vector3 GetDamageScale() const { return sizuku_->GetDamageScale(); }
	int GetDamageId() const { return sizuku_->GetDamageId(); }
	bool IsRainDamaging() const { return sizuku_->IsRainDamaging(); }
	const std::vector<Transform>& GetRainDamageTransforms() const { return sizuku_->GetRainDamageTransforms(); }
	Vector3 GetRainDamageScale() const { return sizuku_->GetRainDamageScale(); }
	int GetRainDamageId() const { return sizuku_->GetRainDamageId(); }
};
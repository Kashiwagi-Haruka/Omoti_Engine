#pragma once
#include "Object/Characters/Playable/Individual/Mei/MeiSpecial.h"
#include "Object/Characters/Playable/Individual/Sizuku/SizukuSpecial.h"
#include "Object3d/Object3d.h"
#include "Transform.h"
#include <memory>
#include <string>
#include <vector>
class PlayerSpecialAttack {

	std::unique_ptr<SizukuSpecial> sizuku_;
	std::unique_ptr<MeiSpecial> mei_;
	bool useMei_ = false;
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
	void SetCharacterName(const std::string& name) { useMei_ = name == "Mei"; }
	bool IsEnd() const { return useMei_ ? mei_->IsEnd() : sizuku_->isEnd(); }
	bool IsAnimationFinished() const { return useMei_ ? mei_->IsAnimationFinished() : sizuku_->IsAnimationFinished(); }
	bool IsDamaging() const { return useMei_ ? mei_->IsDamaging() : sizuku_->IsFlowerDamaging(); }
	Vector3 GetDamagePosition() const { return useMei_ ? mei_->GetDamagePosition() : sizuku_->GetDamagePosition(); }
	Vector3 GetDamageScale() const { return useMei_ ? mei_->GetDamageScale() : sizuku_->GetDamageScale(); }
	int GetDamageId() const { return useMei_ ? mei_->GetDamageId() : sizuku_->GetDamageId(); }
	bool IsRainDamaging() const { return !useMei_ && sizuku_->IsRainDamaging(); }
	const std::vector<Transform>& GetRainDamageTransforms() const { return sizuku_->GetRainDamageTransforms(); }
	Vector3 GetRainDamageScale() const { return sizuku_->GetRainDamageScale(); }
	int GetRainDamageId() const { return sizuku_->GetRainDamageId(); }
};
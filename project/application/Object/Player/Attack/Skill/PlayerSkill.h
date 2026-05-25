#pragma once
#include "Camera.h"
#include "Transform.h"
#include <vector>
#include <memory>
#include "Object/Characters/Playable/Individual/Sizuku/SizukuSkill.h"
class PlayerSkill {

private:
	std::unique_ptr<SizukuSkill> sizuku_;

public:
	PlayerSkill();
	void Initialize();
	void Update();
	void Draw();
	void SetCamera(Camera* camera);
	void StartAttack(const Transform& playerTransform);
	void StartSpecialAttack(const Transform& playerTransform, int iceCount);
	void UpdateSpecialAttack(const Transform& playerTransform);
	void DrawSpecialAttack();
	bool IsSkillEnd() { return isSkillEnd; }
	bool IsDamaging() const { return state == State::damage && !isSkillEnd; }
	bool IsSpecialEnd() const { return isSpecialEnd_; }
	bool IsSpecialDamaging() const { return !isSpecialEnd_; }
	Vector3 GetDamagePosition() const { return damageTransform2_.translate; }
	Vector3 GetDamageScale() const { return damageTransform2_.scale; }
	const std::vector<Transform>& GetSpecialIceFlowerTransforms() const { return iceFlowerTransforms_; }
	int GetSkillDamageId() const { return skillDamageId_; }
};
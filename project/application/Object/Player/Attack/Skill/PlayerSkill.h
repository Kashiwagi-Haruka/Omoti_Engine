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
	bool IsSkillEnd();
	bool IsDamaging()const;
	bool IsSpecialEnd()const;
	bool IsSpecialDamaging()const;
	Vector3 GetDamagePosition()const;
	Vector3 GetDamageScale()const;
	const std::vector<Transform>& GetSpecialIceFlowerTransforms()const;
	int GetSkillDamageId()const;
};
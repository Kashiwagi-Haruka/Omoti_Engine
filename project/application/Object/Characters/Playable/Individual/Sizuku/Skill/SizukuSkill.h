#pragma once
#include "Camera.h"
#include "Engine/math/RigidBody.h"
#include "Object/Characters/Base/Attribute.h"
#include "Object3d/Object3d.h"
#include "ParticleEmitter.h"
#include "SizukuSkillAttributeBase.h"
#include "Transform.h"
#include <memory>
#include <vector>
class SizukuSkillFire;
class SizukuSkillIce;
class SizukuSkillWind;
class SizukuSkillThunder;
class SizukuSkillImaginary;
class SizukuSkillQuantum;

class SizukuSkill {
	friend class SizukuSkillFire;
	friend class SizukuSkillIce;
	friend class SizukuSkillWind;
	friend class SizukuSkillThunder;
	friend class SizukuSkillImaginary;
	friend class SizukuSkillQuantum;

private:
	std::unique_ptr<Object3d> debugBox_;
	std::unique_ptr<Object3d> debugDamageBox1_;
	std::unique_ptr<Object3d> debugDamageBox2_;
	std::unique_ptr<Object3d> skillUpObject_;
	std::unique_ptr<Object3d> skillUnderObject_;
	std::unique_ptr<ParticleEmitter> skillEmitter_;
	Transform particle_;
	Transform transform_;
	Transform damageTransform1_;
	Transform damageTransform2_;
	Transform specialTransform_;
	Camera* camera_ = nullptr;

	float downstartposY = 5;

	bool isSkillEnd = false;
	bool isSpecialEnd_ = true;
	int skillTime = 0;
	int skillTimeMax = 60;
	float upTime = 0;
	float middleTime = 0;
	float downTime = 0;
	float damageTime = 0;
	float endTime = 0;
	int skillDamageId_ = 0;
	std::unique_ptr<std::vector<Object3d>> iceFlowers_;
	std::vector<Transform> iceFlowerTransforms_;
	std::unique_ptr<Object3d> specialDebugBox_;
	float specialRadius_ = 3.0f;
	float specialFallSpeed_ = 0.25f;
	float specialStartHeight_ = 6.0f;
	int specialTime_ = 0;
	int specialTimeMax_ = 60;
	Attribute attribute_ = Attribute::Ice;
	std::unique_ptr<SizukuSkillAttributeBase> fireSkill_;
	std::unique_ptr<SizukuSkillAttributeBase> iceSkill_;
	std::unique_ptr<SizukuSkillAttributeBase> windSkill_;
	std::unique_ptr<SizukuSkillAttributeBase> thunderSkill_;
	std::unique_ptr<SizukuSkillAttributeBase> imaginarySkill_;
	std::unique_ptr<SizukuSkillAttributeBase> quantumSkill_;
	SizukuSkillAttributeBase* activeSkill_ = nullptr;

	void EnsureIceFlowerCount(int count);
	void StartAttributeSkill(const Transform& playerTransform, const Vector4& primaryColor, const Vector4& secondaryColor);
	void UpdateAttributeSkill();

	enum State {
		up,
		middle,
		down,
		damage,
	};
	State state;

public:
	SizukuSkill();
	void Initialize();
	void Update();
	void Draw();
	void SetCamera(Camera* camera);
	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Attribute GetAttribute() const { return attribute_; }
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
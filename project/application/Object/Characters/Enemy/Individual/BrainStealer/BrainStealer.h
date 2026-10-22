#pragma once

#include "Object/Characters/Enemy/Base/BaseEnemy.h"

#include <string>

class EnemyAttack;

class BrainStealer final : public BaseEnemy {
public:
	struct SpecificParameter {
		float playerChaseRange = 8.0f;
		float attackRange = 1.5f;
		float attackCooldown = 0.6f;
	};

	BrainStealer(const EnemyParameter& common, const SpecificParameter& specific);

	~BrainStealer() override;

	static const char* ParameterPath();

	static bool LoadParameter(EnemyParameter& outCommon, SpecificParameter& outSpecific, std::string& error);

	static bool SaveParameter(const EnemyParameter& common, const SpecificParameter& specific, std::string& error);

	bool IsAttacking() const override;
	bool IsAttackHitActive() const override;
	bool ConsumeAttackHit() override;

	Vector3 GetAttackPosition() const override;
	float GetAttackHitSize() const override;

	float GetAttackRange() const override { return specificParameter_.attackRange; }

protected:
	void OnInitialize() override;
	void UpdateTimers() override;

	void UpdateBehavior(const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive) override;

	void UpdateAction() override;
	void DrawAction() override;
	void CancelAction() override;
	void OnCameraChanged() override;

private:
	SpecificParameter specificParameter_;

	std::unique_ptr<EnemyAttack> attack_;

	float attackTimer_ = 0.0f;
};
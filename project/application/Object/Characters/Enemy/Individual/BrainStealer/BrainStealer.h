#pragma once

#include "Object/Characters/Enemy/Base/BaseEnemy.h"
#include "Object/Characters/Enemy/Base/Parameter/EnemyParameter.h"

#include <string>

class BrainStealer final : public BaseEnemy {
public:
	struct SpecificParameter {
		float playerChaseRange = 8.0f;
		float attackRange = 1.5f;
		float attackCooldown = 0.6f;
	};

	BrainStealer(const EnemyParameter& common, const SpecificParameter& specific);

	static const char* ParameterPath();
	static bool LoadParameter(EnemyParameter& outCommon, SpecificParameter& outSpecific, std::string& error);
	static bool SaveParameter(const EnemyParameter& common, const SpecificParameter& specific, std::string& error);

protected:
	void UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) override;

private:
	SpecificParameter specificParameter_;
};
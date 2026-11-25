#pragma once

#include "Object/Characters/Enemy/Base/BaseEnemy.h"

class SwitchEnemy final : public BaseEnemy {
public:
	SwitchEnemy();

protected:
	void UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) override;

private:
	static constexpr float kChaseRange = 8.0f;
	static constexpr float kMoveSpeed = 0.1f;
	static constexpr float kAttackRange = 1.5f;
	static constexpr float kAttackCooldown = 0.6f;
};
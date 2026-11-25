#define NOMINMAX
#include "SwitchEnemy.h"

#include "Function.h"
#include <algorithm>

SwitchEnemy::SwitchEnemy() : BaseEnemy("Enemy", {3.0f, 3.0f, 3.0f}) {}

void SwitchEnemy::UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) {
	const bool targetsPlayer = isPlayerAlive && Function::LengthSquared(playerPosition - GetPosition()) <= kChaseRange * kChaseRange;

	const Vector3& targetPosition = targetsPlayer ? playerPosition : housePosition;
	if (canMove) {
		MoveToward(targetPosition, kMoveSpeed);
	}

	Vector3 toTarget = targetPosition - GetPosition();
	Vector3 toHouse = housePosition - GetPosition();
	toTarget.y = 0.0f;
	toHouse.y = 0.0f;

	const Vector3 scale = GetScale();
	const float enemyRadius = std::max({scale.x, scale.y, scale.z});
	const float houseRadius = std::max({houseScale.x, houseScale.y, houseScale.z});
	const float targetRange = targetsPlayer ? kAttackRange + enemyRadius : kAttackRange;
	const float houseRange = kAttackRange + houseRadius + enemyRadius;

	if (Function::LengthSquared(toTarget) <= targetRange * targetRange || Function::LengthSquared(toHouse) <= houseRange * houseRange) {
		TryStartAttack(kAttackCooldown);
	}
}
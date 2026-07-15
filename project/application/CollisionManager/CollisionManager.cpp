#define NOMINMAX
#include "CollisionManager.h"
#include "Function.h"
#include "Object/Boss/Boss.h"
#include "Object/Characters/Base/Attribute.h"
#include "Object/Characters/Enemy/EnemyManager.h"
#include "Object/Damage/DamageMath.h"
#include "Object/SpecialGaugeBall/SpecialGaugeBallManager.h"
#include "Object/House/House.h"
#include "Object/Player/Player.h"
#include "RigidBody.h"
#include <algorithm>
#include <cmath>
namespace {
AABB MakeAabb(const Vector3& center, const Vector3& halfSize) {
	AABB aabb;
	aabb.min = {center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z};
	aabb.max = {center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z};
	return aabb;
}
bool IsNormalAttackComboStep(int comboStep) { return comboStep >= 1 && comboStep <= 4; }
int CalculateAttributeReactionDamage(const Parameter& playerParameter) {
	const float reactionDamage = (playerParameter.AttributeAffinity * 0.1f) * playerParameter.Attack;
	return std::max(0, static_cast<int>(std::round(reactionDamage)));
}

void ApplyAttributeDamage(Enemy& enemy, EnemyManager& enemyManager, Attribute attribute, const Parameter& playerParameter) {
	if (!enemy.AddAdhesionAttribute(attribute)) {
		return;
	}
	const int attributeReactionDamage = CalculateAttributeReactionDamage(playerParameter);
	enemy.SetHPSubtract(attributeReactionDamage);
	enemyManager.OnEnemyDamaged(&enemy, attributeReactionDamage, attribute, false, enemy.GetLastReactionPreviousAttribute());
}
} // namespace


bool CollisionManager::HandleGameSceneCollisions(
    Player& player, EnemyManager& enemyManager, House& house, Boss* boss, Vector3* outHitEnemyPos, bool* outDidPlayerAttackHitEnemy) {
	bool didNormalAttackHitEnemy = false;
	bool didPlayerAttackHitEnemy = false;
	AABB playerAabb = MakeAabb(player.GetPosition(), player.GetScale());
	AABB houseAabb = MakeAabb(house.GetPosition(), house.GetScale());
	auto tryEnemyFlinch = [](Enemy* target) {
		if (!target->IsAttacking()) {
			target->Stun();
		}
	};

	for (auto& enemy : enemyManager.GetEnemies()) {
		if (!enemy->GetIsAlive() || enemy->IsDying() || enemy->GetHP() <= 0) {
			continue;
		}

		Vector3 enemyPos = enemy->GetPosition();
		AABB enemyAabb = MakeAabb(enemyPos, enemy->GetScale());
		bool hitPlayerBody = player.GetIsAlive() && RigidBody::isCollision(enemyAabb, playerAabb);
		if (hitPlayerBody) {
			Vector3 toEnemy = enemyPos - player.GetPosition();
			toEnemy.y = 0.0f;
			if (Function::LengthSquared(toEnemy) < 0.0001f) {
				toEnemy = {1.0f, 0.0f, 0.0f};
			}
			Vector3 pushDir = Function::Normalize(toEnemy);
			Vector3 playerScale = player.GetScale();
			Vector3 enemyScale = enemy->GetScale();
			float playerRadius = std::max(playerScale.x, playerScale.z);
			float enemyRadius = std::max(enemyScale.x, enemyScale.z);
			float minDistance = playerRadius + enemyRadius;
			Vector3 correctedPos = player.GetPosition() + pushDir * minDistance;
			correctedPos.y = enemyPos.y;
			enemy->SetPosition(correctedPos);
			enemyPos = correctedPos;
			enemyAabb = MakeAabb(enemyPos, enemy->GetScale());
		}
		bool hitHouseBody = RigidBody::isCollision(enemyAabb, houseAabb);
		if (hitHouseBody) {
			Vector3 toEnemy = enemyPos - house.GetPosition();
			toEnemy.y = 0.0f;
			if (Function::LengthSquared(toEnemy) < 0.0001f) {
				toEnemy = {1.0f, 0.0f, 0.0f};
			}
			Vector3 pushDir = Function::Normalize(toEnemy);
			Vector3 houseScale = house.GetScale();
			Vector3 enemyScale = enemy->GetScale();
			float minDistance = houseScale.x + enemyScale.x;
			Vector3 correctedPos = house.GetPosition() + pushDir * minDistance;
			correctedPos.y = enemyPos.y;
			enemy->SetPosition(correctedPos);
			enemyPos = correctedPos;
			enemyAabb = MakeAabb(enemyPos, enemy->GetScale());
		}

		const Attribute playerAttackAttribute = player.GetCurrentAttribute();

		if (player.GetIsAlive() && player.GetSword()->IsAttacking()) {
			Vector3 swordPos = player.GetSword()->GetPosition();
			float swordHit = player.GetSword()->GetHitSize();
			AABB swordAabb = MakeAabb(swordPos, {swordHit, swordHit, swordHit});
			bool hitSword = RigidBody::isCollision(swordAabb, enemyAabb);
			if (hitSword && enemy->CanTakeDamage()) {
				didPlayerAttackHitEnemy = true;
				const int swordComboStep = player.GetSword()->GetComboStep();
				if (IsNormalAttackComboStep(swordComboStep)) {
					didNormalAttackHitEnemy = true;
					if (outHitEnemyPos) {
						*outHitEnemyPos = enemy->GetPosition();
					}
				}
				bool isCritical = false;
				const int damage = DamageMath::CalculatePlayerToEnemyDamage(
				    player.GetCurrentBaseParameter(), player.GetCurrentCombatParameter(), enemy->GetBaseParameter(), enemy->GetParameter(), playerAttackAttribute, &isCritical);
				enemy->SetHPSubtract(damage);
				enemyManager.OnEnemyDamaged(enemy.get(), damage, playerAttackAttribute, isCritical);
				ApplyAttributeDamage(*enemy, enemyManager, playerAttackAttribute, player.GetCurrentCombatParameter());
				if (swordComboStep == 4) {
					enemy->ApplyFinalComboBackStep();
				}
				enemy->TriggerDamageInvincibility();
				tryEnemyFlinch(enemy.get());
			}
		}

		if (player.GetIsAlive() && player.GetSkill() && player.GetSkill()->IsDamaging()) {
			AABB skillAabb = MakeAabb(player.GetSkill()->GetDamagePosition(), player.GetSkill()->GetDamageScale());
			bool hitSkill = RigidBody::isCollision(skillAabb, enemyAabb);
			int skillDamageId = player.GetSkill()->GetSkillDamageId();
			if (hitSkill && enemy->GetLastSkillDamageId() != skillDamageId) {
				didPlayerAttackHitEnemy = true;
				bool isCritical = false;
				const int damage = DamageMath::CalculatePlayerToEnemyDamage(
				    player.GetCurrentBaseParameter(), player.GetCurrentCombatParameter(), enemy->GetBaseParameter(), enemy->GetParameter(), playerAttackAttribute, &isCritical);
				enemy->SetHPSubtract(damage);
				enemyManager.OnEnemyDamaged(enemy.get(), damage, playerAttackAttribute, isCritical);
				ApplyAttributeDamage(*enemy, enemyManager, playerAttackAttribute, player.GetCurrentCombatParameter());
				enemy->SetLastSkillDamageId(skillDamageId);
				tryEnemyFlinch(enemy.get());
			}
		}

		if (player.GetIsAlive() && player.GetSkill() && player.GetSkill()->IsSpecialDamaging()) {
			bool hitSpecial = false;
			for (const auto& specialTransform : player.GetSkill()->GetSpecialIceFlowerTransforms()) {
				AABB specialAabb = MakeAabb(specialTransform.translate, specialTransform.scale);
				if (RigidBody::isCollision(specialAabb, enemyAabb)) {
					hitSpecial = true;
					break;
				}
			}

			if (hitSpecial) {
				if (enemy->CanTakeDamage()) {
					didPlayerAttackHitEnemy = true;
					bool isCritical = false;
					const int damage = DamageMath::CalculatePlayerToEnemyDamage(
					    player.GetCurrentBaseParameter(), player.GetCurrentCombatParameter(), enemy->GetBaseParameter(), enemy->GetParameter(), playerAttackAttribute, &isCritical);
					enemy->SetHPSubtract(damage);
					enemyManager.OnEnemyDamaged(enemy.get(), damage, playerAttackAttribute, isCritical);
					ApplyAttributeDamage(*enemy, enemyManager, playerAttackAttribute, player.GetCurrentCombatParameter());
					enemy->TriggerDamageInvincibility();
					tryEnemyFlinch(enemy.get());
				}
			}
		}

		if (enemy->IsAttackHitActive()) {
			AABB enemyAttackAabb = MakeAabb(enemy->GetAttackPosition(), {enemy->GetAttackHitSize(), enemy->GetAttackHitSize(), enemy->GetAttackHitSize()});
			const bool hitPlayer = RigidBody::isCollision(enemyAttackAabb, playerAabb);
			const bool hitHouse = RigidBody::isCollision(enemyAttackAabb, houseAabb);
			if ((hitPlayer || hitHouse) && enemy->ConsumeAttackHit()) {
				if (hitPlayer) {
					player.Damage(1);
				} else if (hitHouse) {
					house.Damage(1);
				}
			}
		}
	}
	if (boss && boss->GetIsAlive()) {
		AABB bossAabb = MakeAabb(boss->GetPosition(), boss->GetScale());

		if (player.GetIsAlive() && player.GetSword()->IsAttacking()) {
			Vector3 swordPos = player.GetSword()->GetPosition();
			float swordHit = player.GetSword()->GetHitSize();
			AABB swordAabb = MakeAabb(swordPos, {swordHit, swordHit, swordHit});
			bool hitSword = RigidBody::isCollision(swordAabb, bossAabb);
			if (hitSword && boss->CanTakeDamage()) {
				didPlayerAttackHitEnemy = true;
				boss->SetHPSubtract(1);
				boss->TriggerDamageInvincibility();
			}
		}

		if (player.GetIsAlive() && player.GetSkill() && player.GetSkill()->IsDamaging()) {
			AABB skillAabb = MakeAabb(player.GetSkill()->GetDamagePosition(), player.GetSkill()->GetDamageScale());
			bool hitSkill = RigidBody::isCollision(skillAabb, bossAabb);
			int skillDamageId = player.GetSkill()->GetSkillDamageId();
			if (hitSkill && boss->GetLastSkillDamageId() != skillDamageId) {
				didPlayerAttackHitEnemy = true;
				boss->SetHPSubtract(1);
				boss->SetLastSkillDamageId(skillDamageId);
			}
		}

		if (player.GetIsAlive() && player.GetSkill() && player.GetSkill()->IsSpecialDamaging()) {
			bool hitSpecial = false;
			for (const auto& specialTransform : player.GetSkill()->GetSpecialIceFlowerTransforms()) {
				AABB specialAabb = MakeAabb(specialTransform.translate, specialTransform.scale);
				if (RigidBody::isCollision(specialAabb, bossAabb)) {
					hitSpecial = true;
					break;
				}
			}

			if (hitSpecial && boss->CanTakeDamage()) {
				didPlayerAttackHitEnemy = true;
				boss->SetHPSubtract(1);
				boss->TriggerDamageInvincibility();
			}
		}
		if (boss->IsAttackHitActive()) {
			AABB bossAttackAabb{};
			if (boss->IsChargeAttackHitActive()) {
				const Vector3 chargeHitSize = boss->GetChargeAttackHitSize();
				bossAttackAabb = MakeAabb(boss->GetPosition(), chargeHitSize);
			} else {
				const float hitSize = boss->GetAttackHitSize();
				bossAttackAabb = MakeAabb(boss->GetAttackPosition(), {hitSize, hitSize, hitSize});
			}
			const bool hitPlayer = RigidBody::isCollision(bossAttackAabb, playerAabb);
			const bool hitHouse = RigidBody::isCollision(bossAttackAabb, houseAabb);
			if ((hitPlayer || hitHouse) && boss->ConsumeAttackHit()) {
				const int damage = boss->GetAttackDamage();
				if (hitPlayer) {
					player.Damage(damage);
				} else if (hitHouse) {
					house.Damage(damage);
				}
			}
		}
	}

	if (outDidPlayerAttackHitEnemy) {
		*outDidPlayerAttackHitEnemy = didPlayerAttackHitEnemy;
	}
	return didNormalAttackHitEnemy;
}
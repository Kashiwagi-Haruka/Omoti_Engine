#include "DamageMath.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace {
float GetAttributeRate(const std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)>& rates, Attribute attribute) {
	const std::size_t index = static_cast<std::size_t>(attribute);
	if (index >= rates.size()) {
		return 0.0f;
	}
	return rates[index];
}

bool RollCritical(float criticalRate) {
	static std::random_device randomDevice;
	static std::mt19937 randomEngine(randomDevice());
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	return distribution(randomEngine) < std::clamp(criticalRate, 0.0f, 1.0f);
}
} // namespace

void DamageMath::AddDamage(int amount) {
	damage_ += amount;
	if (damage_ > MaxDamage_) {
		damage_ = MaxDamage_;
	}
}
void DamageMath::SubtractDamage(int amount) {
	damage_ -= amount;
	if (damage_ < 0) {
		damage_ = 0;
	}
}

int DamageMath::CalculatePlayerToEnemyDamage(
    const BaseParameter& playerBase, const Parameter& playerParameter, const BaseParameter& enemyBase, const Parameter& enemyParameter, Attribute attackAttribute) {
	const float attack = playerBase.Attack;
	const float defence = enemyBase.Deffence;
	const float attributeDamageRate = GetAttributeRate(playerParameter.AttributeDamageRate, attackAttribute);
	const float attributeResistanceRate = GetAttributeRate(enemyParameter.AttributeResistanceRate, attackAttribute);
	const float criticalDamage = RollCritical(playerParameter.CriticalRate) ? playerParameter.CriticalDamage : 0.0f;

	const float damage = (attack - defence) * (1.0f + (attributeDamageRate - attributeResistanceRate)) * (1.0f + criticalDamage);
	return std::max(0, static_cast<int>(std::round(damage)));
}
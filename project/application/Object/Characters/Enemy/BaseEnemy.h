#pragma once
#include "Characters/Base/Attribute.h"
#include <array>
class BaseEnemy {

	int AttributeAttachmentStatus; // 属性付着状態





};
struct EnemyParameter {
	float HP;
	float Attack;
	float Deffence;
	float Speed;
	float CriticalRate;
	float CriticalDamage;
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeDamageRate{};
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeResistanceRate{};
};
#pragma once
#include <array>
#include "Object/Characters/Base/Attribute.h"
struct BaseParameter {
	float HP;
	float Attack;
	float Deffence;
};
struct Parameter {
	int level;
	float HP;
	float Attack;
	float Deffence;
	float Speed;
	float CriticalRate;
	float CriticalDamage;
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeDamageRate{};
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeResistanceRate{};
};

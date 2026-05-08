#pragma once
#include <array>
#include "Attribute.h"
struct BaseParameter {
	float HP;
	float Attuck;
	float Deffence;
};
struct Parameter {
	int level;
	BaseParameter base;
	float Speed;
	float CriticalRate;
	float CriticalDamage;
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeDamageRate{};
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeResistanceRate{};
};

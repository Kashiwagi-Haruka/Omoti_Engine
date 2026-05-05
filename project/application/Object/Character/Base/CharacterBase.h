#pragma once
#include "Attribute.h"
#include "Engine/math/Transform.h"
class CharacterBase {

protected:
	Transform transform_;
	Attribute attribute_ = Attribute::None;

public:
	CharacterBase() = default;
	virtual ~CharacterBase() = default;

	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Attribute GetAttribute() const { return attribute_; }
};
struct Parameter {
	int HP;
	int Attack;
	int Defense;
	int Speed;
	int CriticalRate;
	int CriticalDamage;
	int AttributeDamageRate;
};
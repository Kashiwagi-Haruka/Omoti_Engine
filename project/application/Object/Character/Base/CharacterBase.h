#pragma once
#include "Engine/math/Transform.h"
class CharacterBase {

	Transform transform_;

	public:
	CharacterBase() = default;


};
struct Parameters {
	int HP;
	int Attack;
	int Defense;
	int Speed;
	int CriticalRate;
	int CriticalDamage;
};

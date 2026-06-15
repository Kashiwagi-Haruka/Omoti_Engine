#pragma once
#include "Object/Characters/Base/Attribute.h"
#include "Object/Characters/Base/CharacterParameters.h"

class DamageMath {

	int MaxDamage_ = 99999999;

	int damage_ = 0;

public:
	void ResetDamage() { damage_ = 0; }
	void AddDamage(int amount);
	void SubtractDamage(int amount);
	static int
	    CalculatePlayerToEnemyDamage(const BaseParameter& playerBase, const Parameter& playerParameter, const BaseParameter& enemyBase, const Parameter& enemyParameter, Attribute attackAttribute);
};
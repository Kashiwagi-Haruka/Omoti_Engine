#pragma once
#include <array>
struct SkilltreeType{

	int normalAttackLv_;
	int skillAttackLv_;
	int specialAttackLv_;
	int AddAbilityLeftLv_;
	int AddAbilityRightLv_;
	std::array<bool, 6> leftSkilltreeLv_;
	std::array<bool, 6> rightSkilltreeLv_;

};


#pragma once
#include "Sprite.h"
#include <memory>
#include <array>
class CharacterDisplaySkilltree {
	std::unique_ptr<Sprite> normalAttackSprite_;
	std::unique_ptr<Sprite> skillAttackSprite_;
	std::unique_ptr<Sprite> specialAttackSprite_;
	std::unique_ptr<Sprite> AddAbilityLeftSprite_;
	std::unique_ptr<Sprite> AddAbilityRightSprite_;
	std::array<std::unique_ptr<Sprite>, 6> leftSkilltreeLv_;
	std::array<std::unique_ptr<Sprite>, 6> rightSkilltreeLv_;
	
public:
	void Initialize();
	void Update();
	void Draw();
};

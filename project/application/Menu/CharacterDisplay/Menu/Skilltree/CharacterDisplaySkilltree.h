#pragma once
#include "Sprite.h"
#include <memory>
#include <array>
#include <string>
#include "Object/Characters/Playable/Base/Arcana.h"
class CharacterDisplaySkilltree {

	static const size_t kMaxskillTreeCount_ = 6;

	std::unique_ptr<Sprite> normalAttackSprite_;
	std::unique_ptr<Sprite> skillAttackSprite_;
	std::unique_ptr<Sprite> specialAttackSprite_;
	std::unique_ptr<Sprite> AddAbilityLeftSprite_;
	std::unique_ptr<Sprite> AddAbilityRightSprite_;
	std::array<std::unique_ptr<Sprite>, kMaxskillTreeCount_> leftSkilltreeSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxskillTreeCount_> rightSkilltreeSprites_;

	std::string selectCharacterName_ = "Sizuku";
	Arcana selectCharacterArcana_ = Arcana::FOOL;
	
public:
	CharacterDisplaySkilltree();
	~CharacterDisplaySkilltree()=default;
	void Initialize();
	void Update();
	void Draw();
	void SetCharacterName(std::string name);
};

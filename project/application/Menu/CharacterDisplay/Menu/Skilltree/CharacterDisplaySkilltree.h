#pragma once
#include "Object/Characters/Playable/Base/Arcana.h"
#include "Object/Characters/Playable/Base/SkilltreeType.h"
#include "Sprite.h"
#include <array>
#include <memory>
#include <string>
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
	SkilltreeType skilltreeType_{};

public:
	CharacterDisplaySkilltree();
	~CharacterDisplaySkilltree() = default;
	void Initialize();
	void Update();
	void Draw();
	void SetCharacterName(std::string name);
	void SetSkilltreeType(const SkilltreeType& skilltreeType);
};
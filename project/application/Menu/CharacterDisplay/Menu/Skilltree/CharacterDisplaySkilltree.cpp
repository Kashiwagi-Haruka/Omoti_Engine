#include "CharacterDisplaySkilltree.h"
CharacterDisplaySkilltree::CharacterDisplaySkilltree() {

	normalAttackSprite_ = std::make_unique<Sprite>();
	skillAttackSprite_ = std::make_unique<Sprite>();
	specialAttackSprite_ = std::make_unique<Sprite>();
	AddAbilityLeftSprite_ = std::make_unique<Sprite>();
	AddAbilityRightSprite_ = std::make_unique<Sprite>();

	for (int i = 0; i < kMaxskillTreeCount_; i++) {
		leftSkilltreeSprites_[i] = std::make_unique<Sprite>();
		rightSkilltreeSprites_[i] = std::make_unique<Sprite>();
	}


}
void CharacterDisplaySkilltree::Initialize() {


}
void CharacterDisplaySkilltree::Update() {



}
void CharacterDisplaySkilltree::Draw() {



}

void CharacterDisplaySkilltree::SetCharacterName(std::string name) { selectCharacterName = name; }
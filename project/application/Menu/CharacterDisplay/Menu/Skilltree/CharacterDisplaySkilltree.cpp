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

	switch (selectCharacterArcana_) {
	case Arcana::FOOL:
		break;
	case Arcana::Magician:
		break;
	case Arcana::HighPriestess:
		break;
	case Arcana::Empress:
		break;
	case Arcana::Emperor:
		break;
	case Arcana::Pope:
		break;
	case Arcana::Lovers:
		break;
	case Arcana::Chariot:
		break;
	case Arcana::Fortitude:
		break;
	case Arcana::Hermit:
		break;
	case Arcana::Fortune:
		break;
	case Arcana::Justice:
		break;
	case Arcana::HangedMan:
		break;
	case Arcana::Death:
		break;
	case Arcana::Temperance:
		break;
	case Arcana::Devile:
		break;
	case Arcana::Tower:
		break;
	case Arcana::Star:
		break;
	case Arcana::Moon:
		break;
	case Arcana::Sun:
		break;
	case Arcana::Judgment:
		break;
	case Arcana::World:
		break;
	default:
		break;
	}

}
void CharacterDisplaySkilltree::Draw() {



}

void CharacterDisplaySkilltree::SetCharacterName(std::string name) { selectCharacterName_ = name; }
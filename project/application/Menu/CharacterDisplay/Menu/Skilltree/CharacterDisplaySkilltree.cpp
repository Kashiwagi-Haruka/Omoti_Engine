#include "CharacterDisplaySkilltree.h"
#include "TextureManager.h"
#include "ScreenSize.h"
#include <numbers>
namespace{
float skillTreeIconSize = 40.0f;
float skillTreeTextureSize = 200.0f;
}
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
	std::string textureName = "Resources/2d/CharacterDIsplay/skillTree/character/" + selectCharacterName_ + ".png";
	uint32_t textureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(textureName);
	uint32_t parameterTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/CharacterDIsplay/skillTree/parameter.png");
	                                     

	normalAttackSprite_->Initialize(textureHandle);
	normalAttackSprite_->SetScale({skillTreeIconSize, skillTreeIconSize});
	normalAttackSprite_->SetTextureRange({skillTreeTextureSize*0.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});

	skillAttackSprite_->Initialize(textureHandle);
	skillAttackSprite_->SetScale({skillTreeIconSize, skillTreeIconSize});
	skillAttackSprite_->SetTextureRange({skillTreeTextureSize * 1.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});
	
	specialAttackSprite_->Initialize(textureHandle);
	specialAttackSprite_->SetScale({skillTreeIconSize, skillTreeIconSize});
	specialAttackSprite_->SetTextureRange({skillTreeTextureSize * 2.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});

	AddAbilityLeftSprite_->Initialize(textureHandle);
	AddAbilityLeftSprite_->SetScale({skillTreeIconSize, skillTreeIconSize});
	AddAbilityLeftSprite_->SetTextureRange({skillTreeTextureSize * 3.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});

	AddAbilityRightSprite_->Initialize(textureHandle);
	AddAbilityRightSprite_->SetScale({skillTreeIconSize, skillTreeIconSize});
	AddAbilityRightSprite_->SetTextureRange({skillTreeTextureSize * 4.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});
	
	for (int i = 0; i < kMaxskillTreeCount_; i++) {
		leftSkilltreeSprites_[i]->Initialize(parameterTextureHandle);
		leftSkilltreeSprites_[i]->SetScale({skillTreeIconSize, skillTreeIconSize});
		leftSkilltreeSprites_[i]->SetTextureRange({0.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});
		rightSkilltreeSprites_[i]->Initialize(parameterTextureHandle);
		rightSkilltreeSprites_[i]->SetScale({skillTreeIconSize, skillTreeIconSize});
		rightSkilltreeSprites_[i]->SetTextureRange({0.0f, 0.0f}, {skillTreeTextureSize, skillTreeTextureSize});
	}
}
void CharacterDisplaySkilltree::Update() {

	switch (selectCharacterArcana_) {
	case Arcana::FOOL:
		AddAbilityLeftSprite_->SetPosition({SCREEN_SIZE::HALF_WIDTH - 60.0f, SCREEN_SIZE::HALF_HEIGHT - 180.0f});
		AddAbilityRightSprite_->SetPosition({SCREEN_SIZE::HALF_WIDTH + 60.0f, SCREEN_SIZE::HALF_HEIGHT - 180.0f});
		skillAttackSprite_->SetPosition({AddAbilityLeftSprite_->GetPosition().x-60.0f, AddAbilityLeftSprite_->GetPosition().y-60.0f});
		normalAttackSprite_->SetPosition({skillAttackSprite_->GetPosition().x - 60.0f, skillAttackSprite_->GetPosition().y - 60.0f});
		specialAttackSprite_->SetPosition({AddAbilityRightSprite_->GetPosition().x+60.0f, AddAbilityRightSprite_->GetPosition().y-60.0f});
		for (int i = 0; i < kMaxskillTreeCount_; i++) {
			leftSkilltreeSprites_[i]->SetPosition(
			    {AddAbilityLeftSprite_->GetPosition().x - (60.0f*sinf(std::numbers::pi_v<float> * ((i + 1.0f) / 6))), AddAbilityLeftSprite_->GetPosition().y + (60.0f * (i + 1))});
			rightSkilltreeSprites_[i]->SetPosition(
			    {AddAbilityRightSprite_->GetPosition().x + (60.0f*sinf(std::numbers::pi_v<float> * ((i + 1.0f) / 6))), AddAbilityLeftSprite_->GetPosition().y + (60.0f * (i + 1))});
		}
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
	normalAttackSprite_->Update();
	skillAttackSprite_->Update();
	specialAttackSprite_->Update();
	AddAbilityLeftSprite_->Update();
	AddAbilityRightSprite_->Update();
	for (int i = 0; i < kMaxskillTreeCount_; i++) {
		leftSkilltreeSprites_[i]->Update();
		rightSkilltreeSprites_[i]->Update();
	}
}
void CharacterDisplaySkilltree::Draw() {

	normalAttackSprite_->Draw();
	skillAttackSprite_->Draw();
	specialAttackSprite_->Draw();
	AddAbilityLeftSprite_->Draw();
	AddAbilityRightSprite_->Draw();
	for (int i = 0; i < kMaxskillTreeCount_; i++) {
		leftSkilltreeSprites_[i]->Draw();
		rightSkilltreeSprites_[i]->Draw();
	}
}

void CharacterDisplaySkilltree::SetCharacterName(std::string name) { selectCharacterName_ = name; }
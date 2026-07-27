#include "CharacterDisplayReinforcement.h"
#include "ScreenSize.h"
#include "TextureManager.h"
namespace{
//中心位置
Vector2 spriteCenterPos = {SCREEN_SIZE::HALF_WIDTH + 100.0f, SCREEN_SIZE::HALF_HEIGHT};
// 比率
Vector2 spriteRatio = {16.0f, 9.0f};
//倍率
float spriteMagnification = 50.0f;
}
CharacterDisplayReinforcement::CharacterDisplayReinforcement(){

	backgroundSprite_ = std::make_unique<Sprite>();

	fullSprite_ = std::make_unique<Sprite>();
	for (int i = 0; i < kMaxSprit_; i++) {
		spritSprites_[i] = std::make_unique<Sprite>();
	}
}
void CharacterDisplayReinforcement::Initialize(){ 
	uint32_t BGHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Reinforcement/Background.png");
	backgroundSprite_->Initialize(BGHandle);
	backgroundSprite_->SetAnchorPoint({0.5f, 0.5f});
	std::string TextureName = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/full.png";
	uint32_t FullHandle = TextureManager::GetInstance() -> GetTextureIndexByfilePath(TextureName);
	fullSprite_->Initialize(FullHandle);
	std::string spritTextureNamefirst = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/1.png";
	std::string spritTextureNamesecond = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/2.png";
	std::string spritTextureNamethird = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/3.png";
	std::string spritTextureNamefour = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/4.png";
	std::string spritTextureNamefive = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/5.png";
	std::string spritTextureNamesix = "Resources/2d/Character/Reinforcement/" + selectCharacterName + "/6.png";
	uint32_t spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamefirst);
	spritSprites_[0]->Initialize(spritTextureNameHandle);
	spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamesecond);
	spritSprites_[1]->Initialize(spritTextureNameHandle);
	spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamethird);
	spritSprites_[2]->Initialize(spritTextureNameHandle);
	spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamefour);
	spritSprites_[3]->Initialize(spritTextureNameHandle);
	spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamefive);
	spritSprites_[4]->Initialize(spritTextureNameHandle);
	spritTextureNameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(spritTextureNamesix);
	spritSprites_[5]->Initialize(spritTextureNameHandle);
	
}
void CharacterDisplayReinforcement::Update() {
	backgroundSprite_->SetPosition(spriteCenterPos);
	backgroundSprite_->SetScale({spriteRatio.x * spriteMagnification, spriteRatio.y * spriteMagnification});
	backgroundSprite_->Update();
	if (haveCount_ = 0) {
		return;
	} else if (haveCount_ = kMaxSprit_) {
		fullSprite_->Update();
	} else {
		for (int i = 0; i <= haveCount_; i++) {
			spritSprites_[i]->Update();
		}
	}

}
void CharacterDisplayReinforcement::Draw() {
	backgroundSprite_->Draw();
	if (haveCount_ = 0) {
		return;
	}else if (haveCount_ = kMaxSprit_) {
		fullSprite_->Draw();
	} else {
		for (int i = 0; i <haveCount_; i++) {
			spritSprites_[i];
		}
	}
	

}

void CharacterDisplayReinforcement::SetReinforcementCount(int count) { haveCount_ = count; }
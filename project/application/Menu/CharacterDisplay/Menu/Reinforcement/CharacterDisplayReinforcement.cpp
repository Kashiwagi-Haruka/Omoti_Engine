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
	for (int i = 0; i < kMaxSprit_; i++) {
	}
	
}
void CharacterDisplayReinforcement::Update() {
	backgroundSprite_->SetPosition(spriteCenterPos);
	backgroundSprite_->SetScale({spriteRatio.x * spriteMagnification, spriteRatio.y * spriteMagnification});
	backgroundSprite_->Update();
	if (haveCount_ = 0) {
		return;
	} else if (haveCount_ = kMaxSprit_) {

	} else {
		for (int i = 0; i <= haveCount_; i++) {
		}
	}

}
void CharacterDisplayReinforcement::Draw() {
	backgroundSprite_->Draw();
	if (haveCount_ = 0) {
		return;
	}else if (haveCount_ = kMaxSprit_) {

	} else {
		for (int i = 0; i <haveCount_; i++) {

		}
	}
	

}
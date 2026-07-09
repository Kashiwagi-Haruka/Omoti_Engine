#include "CharacterDisplayMenuText.h"
#include "SpriteCommon.h"
void CharacterDisplayMenuText::SetMenuType(CharacterDisplayMenuType menuType){ selectMenutype_ = menuType; }

void CharacterDisplayMenuText::Initialize(){
	uint32_t hudFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(hudFontHandle_, 28, 28);
	statusText_.Initialize(hudFontHandle_);
	statusText_.SetSize({400.0f, 60.0f});
	statusText_.SetPosition({50.0f, 140.0f});
	statusText_.SetAlign(TextAlign::Left);
	statusText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	statusText_.SetString(U"ステータス");
	statusText_.UpdateLayout(false);
}

void CharacterDisplayMenuText::Update() { statusText_.Update(); }
void CharacterDisplayMenuText::Draw() { 
	SpriteCommon::GetInstance()->DrawCommonFont();
	statusText_.Draw(); 
}
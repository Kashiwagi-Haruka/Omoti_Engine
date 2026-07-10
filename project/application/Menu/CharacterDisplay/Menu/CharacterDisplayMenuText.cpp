#include "CharacterDisplayMenuText.h"
#include "SpriteCommon.h"

namespace{
	float textPosX = 70.0f;
	float statusPosY = 140.0f;
    float textMarginY = 60.0f;
    }

void CharacterDisplayMenuText::SetMenuType(CharacterDisplayMenuType menuType){ selectMenutype_ = menuType; }

void CharacterDisplayMenuText::Initialize(){
	uint32_t hudFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(hudFontHandle_, 28, 28);
	statusText_.Initialize(hudFontHandle_);
	statusText_.SetSize({400.0f, 60.0f});
	statusText_.SetPosition({textPosX, statusPosY});
	statusText_.SetAlign(TextAlign::Left);
	statusText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	statusText_.SetString(U"ステータス");
	statusText_.UpdateLayout(false);

	weaponText_.Initialize(hudFontHandle_);
	weaponText_.SetSize({400.0f, 60.0f});
	weaponText_.SetPosition({textPosX, statusPosY + textMarginY * static_cast<float>(CharacterDisplayMenuType::WEAPON)});
	weaponText_.SetAlign(TextAlign::Left);
	weaponText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	weaponText_.SetString(U"武器");
	weaponText_.UpdateLayout(false);

	equipText_.Initialize(hudFontHandle_);
	equipText_.SetSize({400.0f, 60.0f});
	equipText_.SetPosition({textPosX, statusPosY + textMarginY * static_cast<float>(CharacterDisplayMenuType::EQUIP)});
	equipText_.SetAlign(TextAlign::Left);
	equipText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	equipText_.SetString(U"装備");
	equipText_.UpdateLayout(false);

	skilltree_.Initialize(hudFontHandle_);
	skilltree_.SetSize({400.0f, 60.0f});
	skilltree_.SetPosition({textPosX, statusPosY + textMarginY * static_cast<float>(CharacterDisplayMenuType::SKILLTREE)});
	skilltree_.SetAlign(TextAlign::Left);
	skilltree_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	skilltree_.SetString(U"鍵強化");
	skilltree_.UpdateLayout(false);

	reinforcement_.Initialize(hudFontHandle_);
	reinforcement_.SetSize({400.0f, 60.0f});
	reinforcement_.SetPosition({textPosX, statusPosY + textMarginY * static_cast<float>(CharacterDisplayMenuType::REINFORCEMENT)});
	reinforcement_.SetAlign(TextAlign::Left);
	reinforcement_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	reinforcement_.SetString(U"断片");
	reinforcement_.UpdateLayout(false);

	profileText_.Initialize(hudFontHandle_);
	profileText_.SetSize({400.0f, 60.0f});
	profileText_.SetPosition({textPosX, statusPosY + textMarginY * static_cast<float>(CharacterDisplayMenuType::PROFILE)});
	profileText_.SetAlign(TextAlign::Left);
	profileText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	profileText_.SetString(U"プロフィール");
	profileText_.UpdateLayout(false);
}

void CharacterDisplayMenuText::Update() { 
	statusText_.Update(); 
	weaponText_.Update();
	equipText_.Update();
	skilltree_.Update();
	reinforcement_.Update();
	profileText_.Update();
}
void CharacterDisplayMenuText::Draw() { 
	SpriteCommon::GetInstance()->DrawCommonFont();
	statusText_.Draw(); 
	weaponText_.Draw();
	equipText_.Draw();
	skilltree_.Draw();
	reinforcement_.Draw();
	profileText_.Draw();
}
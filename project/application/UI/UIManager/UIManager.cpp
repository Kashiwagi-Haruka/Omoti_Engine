#define NOMINMAX
#include "UIManager.h"
#include "GameBase.h"
#include "Object/House/HouseHP.h"
#include "Sprite.h"
#include "Sprite/SpriteCommon.h"
#include <algorithm>

UIManager::UIManager() {

	// ===========================
	//      テクスチャ読み込み
	// ===========================

	hpBarUI_ = std::make_unique<HPBarUI>();

	attackOperationUI_ = std::make_unique<AttackOperation>();

	houseHpPercentSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/houseHPNumbers.png");
	houseHpStringSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/houseHPString.png");

	// ===========================
	//      Sprite の生成
	// ===========================

	houseHpPercentSPData.sprite = std::make_unique<Sprite>();
	for (int i = 0; i < 3; i++) {
		houseHpNumberSPData[i].sprite = std::make_unique<Sprite>();
	}
	houseHpStringSPData.sprite = std::make_unique<Sprite>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	houseHpPercentSPData.sprite->Initialize(houseHpPercentSPData.handle);
	houseHpPercentSPData.sprite->SetTextureRange({houseHpNumbersTextureSize.x * 10.0f, 0}, houseHpNumbersTextureSize);
	houseHpPercentSPData.sprite->SetScale({32, 32});

	houseHpStringSPData.sprite->Initialize(houseHpStringSPData.handle);
	houseHpStringSPData.sprite->SetAnchorPoint({0.5f, 0.5f});
	houseHpStringSPData.sprite->SetScale({160, 90});
	for (int i = 0; i < 3; i++) {
		houseHpNumberSPData[i].sprite->Initialize(houseHpPercentSPData.handle);
		houseHpNumberSPData[i].sprite->SetTextureRange({0, 0}, houseHpNumbersTextureSize);
		houseHpNumberSPData[i].sprite->SetScale({32, 32});
	}
}

void UIManager::Update() {

	hpBarUI_->Update();

	attackOperationUI_->Update();

	int houseMaxHp = HouseHP::GetInstance()->GetMaxHP();
	int houseHp = std::clamp(HouseHP::GetInstance()->GetHP(), 0, houseMaxHp);
	int houseHpPercent = std::clamp(houseHp * 100 / houseMaxHp, 0, 100);
	houseHpDigitStartIndex = houseHpPercent >= 100 ? 0 : (houseHpPercent >= 10 ? 1 : 2);
	int houseDigits[3] = {houseHpPercent / 100, (houseHpPercent / 10) % 10, houseHpPercent % 10};
	float xOffset = houseHpPercentBasePosition.x;
	for (int i = houseHpDigitStartIndex; i < 3; i++) {
		houseHpNumberSPData[i].sprite->SetTextureRange({houseHpNumbersTextureSize.x * houseDigits[i], 0}, houseHpNumbersTextureSize);
		houseHpNumberSPData[i].translate = {xOffset, houseHpPercentBasePosition.y};
		houseHpNumberSPData[i].sprite->SetPosition(houseHpNumberSPData[i].translate);
		houseHpNumberSPData[i].sprite->Update();
		xOffset += 32.0f;
	}
	houseHpPercentSPData.translate = {xOffset, houseHpPercentBasePosition.y};
	houseHpPercentSPData.sprite->SetPosition(houseHpPercentSPData.translate);
	houseHpPercentSPData.sprite->Update();
	float houseHpLabelRight = xOffset + 32.0f;
	Vector2 houseHpLabelCenter = {(houseHpPercentBasePosition.x + houseHpLabelRight) * 0.5f, houseHpPercentBasePosition.y + houseHpStringOffset.y};
	houseHpStringSPData.translate = houseHpLabelCenter;
	houseHpStringSPData.sprite->SetPosition(houseHpStringSPData.translate);
	houseHpStringSPData.sprite->Update();
}

void UIManager::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	hpBarUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	attackOperationUI_->Draw();

	for (int i = houseHpDigitStartIndex; i < 3; i++) {
		houseHpNumberSPData[i].sprite->Draw();
	}
	houseHpPercentSPData.sprite->Draw();
	houseHpStringSPData.sprite->Draw();
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
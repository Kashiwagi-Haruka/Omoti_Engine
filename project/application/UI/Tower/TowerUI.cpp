#include "TowerUI.h"
#include "Object/House/HouseHP.h"
#include "Sprite.h"
#include "TextureManager.h"
#include <algorithm>

TowerUI::TowerUI(){
	houseHpPercentSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/houseHPNumbers.png");
	houseHpStringSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/houseHPString.png");

		houseHpPercentSPData.sprite = std::make_unique<Sprite>();
	for (int i = 0; i < 3; i++) {
		houseHpNumberSPData[i].sprite = std::make_unique<Sprite>();
	}
	houseHpStringSPData.sprite = std::make_unique<Sprite>();
}

TowerUI::~TowerUI() {}

void TowerUI::Initialize() {

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

void TowerUI::Update() {
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

void TowerUI::Draw() {
	for (int i = houseHpDigitStartIndex; i < 3; i++) {
		houseHpNumberSPData[i].sprite->Draw();
	}
	houseHpPercentSPData.sprite->Draw();
	houseHpStringSPData.sprite->Draw();
}
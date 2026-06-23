#include "TowerUI.h"
#include "Object/House/HouseHP.h"
#include "Text/FreetypeManager/FreeTypeManager.h"
#include <algorithm>
#include <string>

TowerUI::TowerUI() = default;

TowerUI::~TowerUI() {}

void TowerUI::Initialize() {
	houseHpFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(houseHpFontHandle_, 32, 32);

	houseHpText_.Initialize(houseHpFontHandle_);
	houseHpText_.SetSize({300.0f, 60.0f});
	houseHpText_.SetPosition(houseHpPercentBasePosition);
	houseHpText_.SetAlign(TextAlign::Left);
	houseHpText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	houseHpText_.SetString(U"100%");
	houseHpText_.UpdateLayout(false);

	houseHpLabelText_.Initialize(houseHpFontHandle_);
	houseHpLabelText_.SetSize({300.0f, 60.0f});
	houseHpLabelText_.SetPosition({houseHpPercentBasePosition.x+houseHpStringOffset.x, houseHpPercentBasePosition.y + houseHpStringOffset.y});
	houseHpLabelText_.SetAlign(TextAlign::Left);
	houseHpLabelText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	houseHpLabelText_.SetString(U"拠点HP");
	houseHpLabelText_.UpdateLayout(false);
}

void TowerUI::Update() {
	int houseMaxHp = HouseHP::GetInstance()->GetMaxHP();
	int houseHpPercent = 0;
	if (houseMaxHp > 0) {
		int houseHp = std::clamp(HouseHP::GetInstance()->GetHP(), 0, houseMaxHp);
		houseHpPercent = std::clamp(houseHp * 100 / houseMaxHp, 0, 100);
	}

	std::u32string houseHpText;
	const std::string percentText = std::to_string(houseHpPercent);
	houseHpText.append(percentText.begin(), percentText.end());
	houseHpText += U"%";
	houseHpText_.SetString(houseHpText);
	houseHpText_.SetPosition(houseHpPercentBasePosition);
	houseHpText_.UpdateLayout(false);
	houseHpText_.Update(false);

	houseHpLabelText_.SetPosition({houseHpPercentBasePosition.x, houseHpPercentBasePosition.y + houseHpStringOffset.y});
	houseHpLabelText_.UpdateLayout(false);
	houseHpLabelText_.Update(false);
}

void TowerUI::Draw() {
	houseHpLabelText_.Draw();
	houseHpText_.Draw();
}
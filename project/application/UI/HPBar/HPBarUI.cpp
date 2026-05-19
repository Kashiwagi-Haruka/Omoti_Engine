#include "HPBarUI.h"
#include "TextureManager.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "Sprite/SpriteCommon.h"
#include <string>
HPBarUI::HPBarUI() {
	playerHpSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/playerHP.png");

	playerHPFlameSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/playerHPFlame.png");

	playerHPStringSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/playerHPString.png");

	playerHpSPData.sprite = std::make_unique<Sprite>();
	playerHPFlameSPData.sprite = std::make_unique<Sprite>();
	playerHPStringSPData.sprite = std::make_unique<Sprite>();
}
void HPBarUI::Initialize() {

	// ------------------ HP Bar ------------------
	playerHpSPData.sprite->Initialize(playerHpSPData.handle);
	playerHpSPData.size = playerHPMaxSize;
	// アンカーポイントを左端に設定（左端固定）
	playerHpSPData.sprite->SetAnchorPoint({0.0f, 0.0f});
	playerHpSPData.sprite->SetScale(playerHpSPData.size);
	// 左端の位置を固定
	playerHpSPData.translate = {640 - playerHPMaxSize.x / 2, 500};
	playerHpSPData.sprite->SetPosition(playerHpSPData.translate);

	// ------------------ HP Flame ------------------
	playerHPFlameSPData.sprite->Initialize(playerHPFlameSPData.handle);
	playerHPFlameSPData.size = playerHPMaxSize;
	playerHPFlameSPData.translate = {640, 500};
	playerHPFlameSPData.sprite->SetAnchorPoint({0.5f, 0.0f});
	playerHPFlameSPData.sprite->SetPosition(playerHPFlameSPData.translate);
	playerHPFlameSPData.sprite->SetScale(playerHPFlameSPData.size);

	// ------------------ HP Label ------------------

	playerHPStringSPData.sprite->Initialize(playerHPStringSPData.handle);
	playerHPStringSPData.sprite->SetAnchorPoint({1.0f, 0.5f});
	playerHPStringSPData.sprite->SetScale({160, 160});
	playerHPStringSPData.translate = {playerHpSPData.translate.x - 10.0f, playerHpSPData.translate.y + playerHPMaxSize.y * 0.5f};
	playerHPStringSPData.sprite->SetPosition(playerHPStringSPData.translate);

	uint32_t fontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(fontHandle_, 64, 64);
	playerHpText.Initialize(fontHandle_);
	playerHpText.SetString(U"HP:0/0");
	playerHpText.SetPosition({playerHPFlameSPData.translate});
	playerHpText.SetColor({1, 1, 1, 1});
	playerHpText.SetAlign(TextAlign::Center);
	playerHpText.UpdateLayout();
}
void HPBarUI::Update() {
	// ---- HP (左端固定、右端から減る) ----
	if (playerHPMax > 0) {
		// HP比率を計算
		float hpRatio = (float)playerHP / (float)playerHPMax;

		// スケールを更新（HPに応じて幅を変更）
		playerHpSPData.size.x = playerHPMaxSize.x * hpRatio;
		playerHpSPData.sprite->SetScale(playerHpSPData.size);

		// テクスチャ範囲を更新（左端0から、HP比率分だけ表示）
		playerHPWidth = playerHPWidthMax * hpRatio;
		playerHpSPData.sprite->SetTextureRange({0, 0}, {playerHPWidth, 300});

		// ★ 左端の位置は常に固定（ずれない）
		playerHpSPData.translate = {640 - playerHPMaxSize.x / 2, 500};
		playerHpSPData.sprite->SetPosition(playerHpSPData.translate);
	}

	// 各スプライト Update
	playerHpSPData.sprite->Update();
	playerHPFlameSPData.sprite->Update();
	playerHPStringSPData.translate = {playerHpSPData.translate.x - 10.0f, playerHpSPData.translate.y + playerHPMaxSize.y * 0.5f};
	playerHPStringSPData.sprite->SetPosition(playerHPStringSPData.translate);
	playerHPStringSPData.sprite->Update();
	std::u32string hpText = U"HP:";
	const std::string currentHPText = std::to_string(playerHP);
	hpText.append(currentHPText.begin(), currentHPText.end());
	hpText += U"/";
	const std::string maxHPText = std::to_string(playerHPMax);
	hpText.append(maxHPText.begin(), maxHPText.end());
	playerHpText.SetString(hpText);
	playerHpText.Update();
}
void HPBarUI::Draw() {
	//playerHpSPData.sprite->Draw();
	//playerHPFlameSPData.sprite->Draw();
	//playerHPStringSPData.sprite->Draw();
	playerHpText.Draw();
}
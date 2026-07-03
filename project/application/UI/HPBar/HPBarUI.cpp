#include "HPBarUI.h"
#include "TextureManager.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "Sprite/SpriteCommon.h"
#include <string>
HPBarUI::HPBarUI() {

	playerHpSPData.sprite = std::make_unique<Sprite>();
	playerHPFlameSPData.sprite = std::make_unique<Sprite>();
	playerHpBarBackgroundSprite.sprite = std::make_unique<Sprite>();
}
void HPBarUI::Initialize() {
	uint32_t playerHpBarHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/HPBar/HP.png");
	uint32_t playerHpBarBackgroundHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/HPBar/HPBackground.png");
	uint32_t playerHPFlameHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/HPBar/HPFlame.png");
	// ------------------ HP Bar ------------------
	playerHpSPData.sprite->Initialize(playerHpBarHandle);
	playerHpSPData.size = playerHPMaxSize;
	// アンカーポイントを左端に設定（左端固定）
	playerHpSPData.sprite->SetAnchorPoint({0.0f, 0.5f});
	playerHpSPData.sprite->SetScale(playerHpSPData.size);
	// 左端の位置を固定
	playerHpSPData.translate = {640 - playerHPMaxSize.x / 2, 600};
	playerHpSPData.sprite->SetPosition(playerHpSPData.translate);

	// ------------------ HP Flame ------------------
	playerHPFlameSPData.sprite->Initialize(playerHPFlameHandle);
	playerHPFlameSPData.size = {playerHPMaxSize.x*13.0f/12.0f,playerHPMaxSize.y*11.0f/6.0f};
	playerHPFlameSPData.translate = {640, 600};
	playerHPFlameSPData.sprite->SetAnchorPoint({0.5f, 0.5f});
	playerHPFlameSPData.sprite->SetPosition(playerHPFlameSPData.translate);
	playerHPFlameSPData.sprite->SetScale(playerHPFlameSPData.size);

	// ------------------ HP Background ------------------
	playerHpBarBackgroundSprite.sprite->Initialize(playerHpBarBackgroundHandle);
	playerHpBarBackgroundSprite.size = playerHPMaxSize;
	playerHpBarBackgroundSprite.translate = {640 - playerHPMaxSize.x / 2, 600};
	playerHpBarBackgroundSprite.sprite->SetAnchorPoint({0.0f, 0.5f});
	playerHpBarBackgroundSprite.sprite->SetPosition(playerHpBarBackgroundSprite.translate);
	playerHpBarBackgroundSprite.sprite->SetScale(playerHpBarBackgroundSprite.size);

	// ------------------ HP Label ------------------

	uint32_t fontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(fontHandle_, 32, 32);
	playerHpText.Initialize(fontHandle_);
	playerHpText.SetString(U"HP");
	playerHpText.SetPosition({playerHPFlameSPData.translate.x,playerHPFlameSPData.translate.y+10.0f});
	playerHpText.SetColor({0.0f, 0.0f, 0.2f, 1.0f});
	playerHpText.SetAlign(TextAlign::Center);
	playerHpText.UpdateLayout(false);
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
		playerHpSPData.translate.x = 640 - playerHPMaxSize.x / 2;
		playerHpSPData.sprite->SetPosition(playerHpSPData.translate);
	}

	// 各スプライト Update
	playerHpSPData.sprite->Update();
	playerHPFlameSPData.sprite->Update();
	playerHpBarBackgroundSprite.sprite->Update();
	std::u32string hpText = U"HP:";
	const std::string currentHPText = std::to_string(playerHP);
	hpText.append(currentHPText.begin(), currentHPText.end());
	hpText += U"/";
	const std::string maxHPText = std::to_string(playerHPMax);
	hpText.append(maxHPText.begin(), maxHPText.end());
	playerHpText.SetString(hpText);
	playerHpText.UpdateLayout(false);
}
void HPBarUI::Draw() {
	playerHpBarBackgroundSprite.sprite->Draw();
	playerHpSPData.sprite->Draw();
	playerHPFlameSPData.sprite->Draw();
	playerHpText.Draw();
}
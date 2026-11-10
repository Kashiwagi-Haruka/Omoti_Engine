#include "ResultScene.h"
#include "Input.h"
#include "GameTimer/GameTimer.h"
#include "Object/House/HouseHP.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include <algorithm>
#include "AudioManager/BGMManager/BGMManager.h"

namespace {
constexpr float kGoodTimeSeconds = 120.0f;
constexpr float kOkTimeSeconds = 180.0f;
constexpr float kHighHpRatio = 0.8f;
constexpr float kMidHpRatio = 0.5f;
} // namespace

int CalculateStarCount(float timeSeconds, int houseHp) {
	float hpRatio = std::clamp(houseHp / static_cast<float>(HouseHP::GetInstance()->GetMaxHP()), 0.0f, 1.0f);
	int stars = 1;
	if (timeSeconds <= kGoodTimeSeconds && hpRatio >= kHighHpRatio) {
		stars = 3;
	} else if (timeSeconds <= kOkTimeSeconds && hpRatio >= kMidHpRatio) {
		stars = 2;
	}
	return stars;
}
ResultScene::ResultScene() {

	logoSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/result.png");
	logoSpriteData_.sprite = std::make_unique<Sprite>();
	logoSpriteData_.sprite->Initialize(logoSpriteData_.handle);
	starOnHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/StarOn.png");
	starOffHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/StarOff.png");
	numberHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/No.png");
	houseHpStringHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/houseHPString.png");

	for (int i = 0; i < 4; ++i) {
		timeDigitSpriteData_[i].handle = numberHandle_;
		timeDigitSpriteData_[i].sprite = std::make_unique<Sprite>();
		timeDigitSpriteData_[i].sprite->Initialize(timeDigitSpriteData_[i].handle);
	}
	houseHpStringSpriteData_.handle = houseHpStringHandle_;
	houseHpStringSpriteData_.sprite = std::make_unique<Sprite>();
	houseHpStringSpriteData_.sprite->Initialize(houseHpStringSpriteData_.handle);

	transition = std::make_unique<SceneTransition>();
}

void ResultScene::Finalize() {}

void ResultScene::Initialize() {
	logoSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	logoSpriteData_.size = {1280, 720};
	logoSpriteData_.translate = {640, 360};
	logoSpriteData_.sprite->SetScale(logoSpriteData_.size);

	logoSpriteData_.sprite->SetPosition(logoSpriteData_.translate);
	logoSpriteData_.sprite->Update();

	isSceneEnd_ = false;
	// SPACE 画像読み込み
	pressSpaceHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/SPACE.png");

	pressSpaceSprite = std::make_unique<Sprite>();
	pressSpaceSprite->Initialize(pressSpaceHandle);

	// 中央寄せ
	pressSpaceSprite->SetAnchorPoint({0.5f, 0.5f});
	pressSpaceSprite->SetTextureRange({0, 0}, {768, 768});
	// 300 × 300
	pressSpaceSprite->SetScale(pressSpaceSize);

	// 画面中央より少し下（Y = 360 より下）
	pressSpaceSprite->SetPosition(pressSpacePos);

	pressSpaceSprite->Update();
	float totalSeconds = GameTimer::GetInstance()->GetTimer();
	resultMinutes_ = static_cast<int>(totalSeconds) / 60;
	resultSeconds_ = static_cast<int>(totalSeconds) % 60;
	resultStars_ = CalculateStarCount(totalSeconds, HouseHP::GetInstance()->GetHP());

	Vector2 starStart = {460, 360};
	float starSpacing = 180.0f;
	for (int i = 0; i < 3; ++i) {
		starSpriteData_[i].handle = (i < resultStars_) ? starOnHandle_ : starOffHandle_;
		starSpriteData_[i].sprite = std::make_unique<Sprite>();
		starSpriteData_[i].sprite->Initialize(starSpriteData_[i].handle);
		starSpriteData_[i].sprite->SetAnchorPoint({0.5f, 0.5f});
		starSpriteData_[i].size = starSize_;
		starSpriteData_[i].translate = {starStart.x + starSpacing * i, starStart.y};
		starSpriteData_[i].sprite->SetScale(starSpriteData_[i].size);
		starSpriteData_[i].sprite->SetPosition(starSpriteData_[i].translate);
		starSpriteData_[i].sprite->Update();
	}
	Vector2 starTopLeft = {starStart.x - starSize_.x*3.0f, starStart.y+80.0f};
	houseHpStringSpriteData_.sprite->SetAnchorPoint({0.0f, 1.0f});
	houseHpStringSpriteData_.size = houseHpStringSize_;
	houseHpStringSpriteData_.translate = starTopLeft;
	houseHpStringSpriteData_.sprite->SetScale(houseHpStringSpriteData_.size);
	houseHpStringSpriteData_.sprite->SetPosition(houseHpStringSpriteData_.translate);
	houseHpStringSpriteData_.sprite->Update();
	int minuteTens = (resultMinutes_ / 10) % 10;
	int minuteOnes = resultMinutes_ % 10;
	int secondTens = (resultSeconds_ / 10) % 10;
	int secondOnes = resultSeconds_ % 10;

	int digits[4] = {minuteTens, minuteOnes, secondTens, secondOnes};
	Vector2 digitStart = {580, 280};
	float digitSpacing = 50.0f;
	for (int i = 0; i < 4; ++i) {
		timeDigitSpriteData_[i].sprite->SetAnchorPoint({0.5f, 0.5f});
		timeDigitSpriteData_[i].size = timeDigitSize_;
		timeDigitSpriteData_[i].translate = {digitStart.x + digitSpacing * i, digitStart.y};
		timeDigitSpriteData_[i].sprite->SetScale(timeDigitSpriteData_[i].size);
		timeDigitSpriteData_[i].sprite->SetPosition(timeDigitSpriteData_[i].translate);
		timeDigitSpriteData_[i].sprite->SetTextureRange({300.0f * digits[i], 0}, numberTextureSize_);
		timeDigitSpriteData_[i].sprite->Update();
	}

	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
	transition->Initialize(false);
	isTransitionIn = true;
	isTransitionOut = false;
}
void ResultScene::Update() {
	BGMManager::GetInstance()->Play(BGMManager::BGMType::Result);
	logoSpriteData_.sprite->Update();
	for (int i = 0; i < 3; ++i) {
		starSpriteData_[i].sprite->Update();
	}
	for (int i = 0; i < 4; ++i) {
		timeDigitSpriteData_[i].sprite->Update();
	}
	houseHpStringSpriteData_.sprite->Update();
	if ((Input::GetInstance()->TriggerKey(DIK_SPACE)||Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA)) && !isTransitionOut) {
		transition->Initialize(true);
		isTransitionOut = true;
	}
	if (isTransitionIn || isTransitionOut) {
		transition->Update();
		if (transition->IsEnd() && isTransitionIn) {
			isTransitionIn = false;
		}
		if (transition->IsEnd() && isTransitionOut) {
			SceneManager::GetInstance()->ChangeScene("Title");
		}
	}
#ifdef USE_IMGUI
	ImGui::Begin("resultScene");
	ImGui::End();
#endif // USE_IMGUI
}

void ResultScene::Draw() {
	SpriteCommon::GetInstance()->DrawCommon();
	logoSpriteData_.sprite->Draw();
	for (int i = 0; i < 3; ++i) {
		starSpriteData_[i].sprite->Draw();
	}
	for (int i = 0; i < 4; ++i) {
		timeDigitSpriteData_[i].sprite->Draw();
	}
	houseHpStringSpriteData_.sprite->Draw();
	pressSpaceSprite->Draw();
	if (isTransitionIn || isTransitionOut) {
		transition->Draw();
	}
}
#include "TeamUI.h"
#include "Function.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <algorithm>
namespace {
constexpr Vector2 kIconSize{35.0f, 35.0f};
constexpr Vector2 kActiveIconSize{40.0f, 40.0f};
constexpr Vector2 kPadIconSize{28.0f, 28.0f};
constexpr float kPadIconGap = 8.0f;
constexpr Vector2 kHpBarMaxSize{30.0f, 4.0f};
constexpr Vector2 kHpBarOffset{-kIconSize.x * 1.3f, kIconSize.y * 0.9f};
constexpr Vector2 kCharacterNameOffset{-kIconSize.x * 1.3f, 16.0f};
constexpr Vector2 kCharacterNameTextSize{120.0f, 24.0f};
constexpr float kRightMargin = 20.0f + kPadIconSize.x + kPadIconGap;
constexpr float kTopMargin = 160.0f;
constexpr float kIconSpacing = 14.0f;
constexpr Vector2 kRightTopAnchor{1.0f, 0.0f};
constexpr Vector2 kLeftTopAnchor{0.0f, 0.0f};
constexpr Vector4 kActiveColor{1.0f, 1.0f, 1.0f, 1.0f};
constexpr Vector4 kInactiveColor{0.65f, 0.65f, 0.65f, 0.78f};
constexpr Vector4 kCharacterNameColor{1.0f, 1.0f, 1.0f, 1.0f};
constexpr Vector4 kInactiveCharacterNameColor{0.75f, 0.75f, 0.75f, 0.78f};
constexpr std::array<const char*, Team::kMaxMembersCount> kPadTexturePaths{
    "Resources/2d/Team/UI/Pad/UP.png",
    "Resources/2d/Team/UI/Pad/RIGHT.png",
    "Resources/2d/Team/UI/Pad/DOWN.png",
    "Resources/2d/Team/UI/Pad/LEFT.png",
};
constexpr std::array<const char*, Team::kMaxMembersCount> kKeyboardTexturePaths{
    "Resources/2d/Team/UI/KeyBoard/1.png",
    "Resources/2d/Team/UI/KeyBoard/2.png",
    "Resources/2d/Team/UI/KeyBoard/3.png",
    "Resources/2d/Team/UI/KeyBoard/4.png",
};
} // namespace

TeamUI::TeamUI() { displayedCharacterIndices_.fill(-1); }

TeamUI::~TeamUI() = default;

void TeamUI::Initialize(const Team& team) {
	uint32_t hpBarTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBar.png");
	uint32_t hpBarBackgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBarBackground.png");
	uint32_t specialGaugeTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/specialGauge.png");
	uint32_t specialGaugeFlameTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/special.png");
	uint32_t selectedBackgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/SelectedBackground.png");
	std::array<uint32_t, kMaxMembersCount> padTextureHandles{};
	for (int i = 0; i < kMaxMembersCount; ++i) {
		padTextureHandles[i] = TextureManager::GetInstance()->GetTextureIndexByfilePath(kPadTexturePaths[i]);
	}
	std::array<uint32_t, kMaxMembersCount> keyboardTextureHandles{};
	for (int i = 0; i < kMaxMembersCount; ++i) {
		keyboardTextureHandles[i] = TextureManager::GetInstance()->GetTextureIndexByfilePath(kKeyboardTexturePaths[i]);
	}

	characterNameFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(characterNameFontHandle_, 14, 14);

	for (int i = 0; i < kMaxMembersCount; ++i) {
		iconPositions_[i] = {
		    static_cast<float>(WinApp::kClientWidth) - kRightMargin,
		    kTopMargin + (kIconSize.y + kIconSpacing) * static_cast<float>(i),
		};

		characterNameTexts_[i].Initialize(characterNameFontHandle_);
		characterNameTexts_[i].SetSize(kCharacterNameTextSize);
		characterNameTexts_[i].SetAlign(TextAlign::Right);

		const int characterIndex = team.GetMemberCharacterIndex(i);
		displayedCharacterIndices_[i] = characterIndex;
		if (!team.GetHasMember(i) || characterIndex < 0) {
			characterNameTexts_[i].SetString(U"");
			characterNameTexts_[i].UpdateLayout(false);
			iconSprites_[i].reset();
			hpBarSprites_[i].reset();
			hpBarBackgroundSprites_[i].reset();
			padSprites_[i].reset();
			keyboardSprites_[i].reset();
			specialGaugeFlameSprites_[i].reset();
			specialGaugeSprites_[i].reset();
			continue;
		}

		iconSprites_[i] = std::make_unique<Sprite>();
		iconSprites_[i]->Initialize(team.GetOwnedCharacterIconHandle(characterIndex));
		iconSprites_[i]->SetAnchorPoint(kRightTopAnchor);
		iconSprites_[i]->SetPosition(iconPositions_[i]);
		iconSprites_[i]->SetScale(i == team.GetActiveSlot() ? kActiveIconSize : kIconSize);
		iconSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		iconSprites_[i]->Update();

		padSprites_[i] = std::make_unique<Sprite>();
		padSprites_[i]->Initialize(padTextureHandles[i]);
		padSprites_[i]->SetAnchorPoint(kLeftTopAnchor);
		padSprites_[i]->SetPosition(iconPositions_[i] + Vector2{kPadIconGap, 0.0f});
		padSprites_[i]->SetScale(kPadIconSize);
		padSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		padSprites_[i]->Update();

		keyboardSprites_[i] = std::make_unique<Sprite>();
		keyboardSprites_[i]->Initialize(keyboardTextureHandles[i]);
		keyboardSprites_[i]->SetAnchorPoint(kLeftTopAnchor);
		keyboardSprites_[i]->SetPosition(iconPositions_[i] + Vector2{kPadIconGap, 0.0f});
		keyboardSprites_[i]->SetScale(kPadIconSize);
		keyboardSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		keyboardSprites_[i]->Update();

		hpBarSprites_[i] = std::make_unique<Sprite>();
		hpBarSprites_[i]->Initialize(hpBarTextureHandle);
		hpBarSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		hpBarSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset);
		{
			const int hpMax = team.GetMemberHPMax(i);
			const float hpRate = hpMax > 0 ? std::clamp(static_cast<float>(team.GetMemberHP(i)) / static_cast<float>(hpMax), 0.0f, 1.0f) : 0.0f;
			hpBarSprites_[i]->SetScale({kHpBarMaxSize.x * hpRate, kHpBarMaxSize.y});
		}
		hpBarSprites_[i]->Update();

		hpBarBackgroundSprites_[i] = std::make_unique<Sprite>();
		hpBarBackgroundSprites_[i]->Initialize(hpBarBackgroundTextureHandle);
		hpBarBackgroundSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		hpBarBackgroundSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset);
		hpBarBackgroundSprites_[i]->SetScale(kHpBarMaxSize);
		hpBarBackgroundSprites_[i]->Update();

		specialGaugeFlameSprites_[i] = std::make_unique<Sprite>();
		specialGaugeFlameSprites_[i]->Initialize(specialGaugeFlameTextureHandle);
		specialGaugeFlameSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		specialGaugeFlameSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset + Vector2{-kHpBarMaxSize.x * 1.2f, -kHpBarOffset.y * 0.4f});
		specialGaugeFlameSprites_[i]->SetScale(kIconSize);
		specialGaugeFlameSprites_[i]->Update();

		specialGaugeSprites_[i] = std::make_unique<Sprite>();
		specialGaugeSprites_[i]->Initialize(specialGaugeTextureHandle);
		specialGaugeSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		specialGaugeSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset + Vector2{-kHpBarMaxSize.x * 1.2f, -kHpBarOffset.y * 0.4f});
		specialGaugeSprites_[i]->SetScale(kIconSize);
		specialGaugeSprites_[i]->Update();

		characterNameTexts_[i].SetString(team.GetCharacterNameByIndex(characterIndex));
		characterNameTexts_[i].SetPosition(iconPositions_[i] + kCharacterNameOffset);
		characterNameTexts_[i].SetColor(i == team.GetActiveSlot() ? kCharacterNameColor : kInactiveCharacterNameColor);
		characterNameTexts_[i].UpdateLayout(false);
	}

	selectedBackgroundSprite_ = std::make_unique<Sprite>();
	selectedBackgroundSprite_->Initialize(selectedBackgroundTextureHandle);
	selectedBackgroundSprite_->SetAnchorPoint({1.0f, 0.0f});
	selectedBackgroundSprite_->SetScale({kActiveIconSize.x + kHpBarMaxSize.x + (-kHpBarOffset.x), kActiveIconSize.y});
	selectedBackgroundSprite_->SetPosition(iconPositions_[team.GetActiveSlot()]);
	selectedBackgroundSprite_->SetColor({0.4f, 0.4f, 0.5f, 0.7f});
	selectedBackgroundSprite_->Update();
}

void TeamUI::Update(const Team& team) {
	UpdateInputDisplayMode();
	for (int i = 0; i < kMaxMembersCount; ++i) {
		const int characterIndex = team.GetMemberCharacterIndex(i);
		if (!team.GetHasMember(i) || characterIndex < 0) {
			characterNameTexts_[i].SetString(U"");
			characterNameTexts_[i].UpdateLayout(false);
			iconSprites_[i].reset();
			hpBarSprites_[i].reset();
			hpBarBackgroundSprites_[i].reset();
			padSprites_[i].reset();
			keyboardSprites_[i].reset();
			specialGaugeFlameSprites_[i].reset();
			specialGaugeSprites_[i].reset();
			displayedCharacterIndices_[i] = -1;
			continue;
		}

		if (!iconSprites_[i]) {
			iconSprites_[i] = std::make_unique<Sprite>();
			iconSprites_[i]->Initialize(team.GetOwnedCharacterIconHandle(characterIndex));
			iconSprites_[i]->SetAnchorPoint(kRightTopAnchor);
		} else if (displayedCharacterIndices_[i] != characterIndex) {
			iconSprites_[i]->Initialize(team.GetOwnedCharacterIconHandle(characterIndex));
			iconSprites_[i]->SetAnchorPoint(kRightTopAnchor);
		}

		if (!padSprites_[i]) {
			const uint32_t padTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(kPadTexturePaths[i]);
			padSprites_[i] = std::make_unique<Sprite>();
			padSprites_[i]->Initialize(padTextureHandle);
			padSprites_[i]->SetAnchorPoint(kLeftTopAnchor);
		}

		if (!keyboardSprites_[i]) {
			const uint32_t keyboardTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(kKeyboardTexturePaths[i]);
			keyboardSprites_[i] = std::make_unique<Sprite>();
			keyboardSprites_[i]->Initialize(keyboardTextureHandle);
			keyboardSprites_[i]->SetAnchorPoint(kLeftTopAnchor);
		}

		if (!hpBarSprites_[i]) {
			const uint32_t hpBarTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBar.png");
			hpBarSprites_[i] = std::make_unique<Sprite>();
			hpBarSprites_[i]->Initialize(hpBarTextureHandle);
			hpBarSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		}
		if (!hpBarBackgroundSprites_[i]) {
			const uint32_t hpBarBackgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBarBackground.png");
			hpBarBackgroundSprites_[i] = std::make_unique<Sprite>();
			hpBarBackgroundSprites_[i]->Initialize(hpBarBackgroundTextureHandle);
			hpBarBackgroundSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		}
		if (!specialGaugeFlameSprites_[i]) {
			const uint32_t specialGaugeFlameTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/special.png");
			specialGaugeFlameSprites_[i] = std::make_unique<Sprite>();
			specialGaugeFlameSprites_[i]->Initialize(specialGaugeFlameTextureHandle);
			specialGaugeFlameSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		}
		if (!specialGaugeSprites_[i]) {
			const uint32_t specialGaugeTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/specialGauge.png");
			specialGaugeSprites_[i] = std::make_unique<Sprite>();
			specialGaugeSprites_[i]->Initialize(specialGaugeTextureHandle);
			specialGaugeSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		}

		displayedCharacterIndices_[i] = characterIndex;
		iconSprites_[i]->SetPosition(iconPositions_[i]);
		iconSprites_[i]->SetScale(i == team.GetActiveSlot() ? kActiveIconSize : kIconSize);
		iconSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		iconSprites_[i]->Update();

		padSprites_[i]->SetPosition(iconPositions_[i] + Vector2{kPadIconGap, 0.0f});
		padSprites_[i]->SetScale(kPadIconSize);
		padSprites_[i]->SetColor(i == team.GetActiveSlot() ? kInactiveColor : kActiveColor);
		padSprites_[i]->Update();

		keyboardSprites_[i]->SetPosition(iconPositions_[i] + Vector2{kPadIconGap, 0.0f});
		keyboardSprites_[i]->SetScale(kPadIconSize);
		keyboardSprites_[i]->SetColor(i == team.GetActiveSlot() ? kInactiveColor : kActiveColor);
		keyboardSprites_[i]->Update();		

		hpBarSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset);
		{
			const int hpMax = team.GetMemberHPMax(i);
			const float hpRate = hpMax > 0 ? std::clamp(static_cast<float>(team.GetMemberHP(i)) / static_cast<float>(hpMax), 0.0f, 1.0f) : 0.0f;
			hpBarSprites_[i]->SetScale({kHpBarMaxSize.x * hpRate, kHpBarMaxSize.y});
		}
		hpBarSprites_[i]->Update();

		hpBarBackgroundSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset);
		hpBarBackgroundSprites_[i]->SetScale(kHpBarMaxSize);
		hpBarBackgroundSprites_[i]->Update();

		const Vector2 specialGaugePosition = iconPositions_[i] + kHpBarOffset + Vector2{-kHpBarMaxSize.x * 1.2f, -kHpBarOffset.y * 0.4f};
		specialGaugeFlameSprites_[i]->SetPosition(specialGaugePosition);
		specialGaugeFlameSprites_[i]->SetScale(kIconSize);
		specialGaugeFlameSprites_[i]->Update();

		specialGaugeSprites_[i]->SetPosition(specialGaugePosition);
		specialGaugeSprites_[i]->SetScale(kIconSize);
		specialGaugeSprites_[i]->Update();

		characterNameTexts_[i].SetString(team.GetCharacterNameByIndex(characterIndex));
		characterNameTexts_[i].SetPosition(iconPositions_[i] + kCharacterNameOffset);
		characterNameTexts_[i].SetColor(i == team.GetActiveSlot() ? kCharacterNameColor : kInactiveCharacterNameColor);
		characterNameTexts_[i].UpdateLayout(false);
		characterNameTexts_[i].Update(false);
	}
	selectedBackgroundSprite_->SetPosition(iconPositions_[team.GetActiveSlot()]);
	selectedBackgroundSprite_->Update();
}

void TeamUI::Draw() {
	selectedBackgroundSprite_->Draw();
	for (auto& iconSprite : iconSprites_) {
		if (iconSprite) {
			iconSprite->Draw();
		}
	}
	if (inputDisplayMode_ == InputDisplayMode::Pad) {
		for (auto& padSprite : padSprites_) {
			if (padSprite) {
				padSprite->Draw();
			}
		}
	}
	if (inputDisplayMode_ == InputDisplayMode::Keyboard) {
		for (auto& keyboardSprite : keyboardSprites_) {
			if (keyboardSprite) {
				keyboardSprite->Draw();
			}
		}
	}
	
	for (auto& characterNameText : characterNameTexts_) {
		characterNameText.Draw();
	}
	SpriteCommon::GetInstance()->DrawCommon();
	for (auto& hpBarBackgroundSprite : hpBarBackgroundSprites_) {
		if (hpBarBackgroundSprite) {
			hpBarBackgroundSprite->Draw();
		}
	}
	for (auto& hpBarSprite : hpBarSprites_) {
		if (hpBarSprite) {
			hpBarSprite->Draw();
		}
	}
	for (auto& specialGaugeSprite : specialGaugeSprites_) {
		if (specialGaugeSprite) {
			specialGaugeSprite->Draw();
		}
	}
	for (auto& specialGaugeFlameSprite : specialGaugeFlameSprites_) {
		if (specialGaugeFlameSprite) {
			specialGaugeFlameSprite->Draw();
		}
	}
}
void TeamUI::UpdateInputDisplayMode() {
	Input* input = Input::GetInstance();

	const bool isPadInput = input->PushButton(Input::PadButton::kButtonA) || input->PushButton(Input::PadButton::kButtonB) || input->PushButton(Input::PadButton::kButtonX) ||
	                        input->PushButton(Input::PadButton::kButtonY) || input->PushButton(Input::PadButton::kButtonLeftShoulder) || input->PushButton(Input::PadButton::kButtonRightShoulder) ||
	                        input->PushButton(Input::PadButton::kButtonBack) || input->PushButton(Input::PadButton::kButtonStart) || input->PushButton(Input::PadButton::kButtonLeftThumb) ||
	                        input->PushButton(Input::PadButton::kButtonRightThumb) || input->PushButton(Input::PadButton::kButtonUp) || input->PushButton(Input::PadButton::kButtonDown) ||
	                        input->PushButton(Input::PadButton::kButtonLeft) || input->PushButton(Input::PadButton::kButtonRight) || input->PushLeftTrigger() || input->PushRightTrigger();
	if (isPadInput) {
		inputDisplayMode_ = InputDisplayMode::Pad;
		return;
	}

	const bool isKeyboardInput = input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_SPACE) || input->PushKey(DIK_E) ||
	                             input->PushKey(DIK_Q) || input->PushKey(DIK_ESCAPE) || input->PushKey(DIK_RETURN) || input->PushKey(DIK_1) || input->PushKey(DIK_2) || input->PushKey(DIK_3) ||
	                             input->PushKey(DIK_4) || input->PushMouseButton(Input::MouseButton::kLeft) || input->PushMouseButton(Input::MouseButton::kRight);
	if (isKeyboardInput) {
		inputDisplayMode_ = InputDisplayMode::Keyboard;
	}
}
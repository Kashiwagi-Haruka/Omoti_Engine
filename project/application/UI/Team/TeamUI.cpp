#include "TeamUI.h"
#include "WinApp.h"
#include "TextureManager.h"
#include "Function.h"
namespace {
constexpr Vector2 kIconSize{35.0f, 35.0f};
constexpr Vector2 kActiveIconSize{40.0f, 40.0f};
constexpr Vector2 kHpBarMaxSize{30.0f, 4.0f};
constexpr Vector2 kHpBarOffset{-kIconSize.x*1.3f, kIconSize.y * 0.9f};
constexpr float kRightMargin = 20.0f;
constexpr float kTopMargin = 160.0f;
constexpr float kIconSpacing = 14.0f;
constexpr Vector2 kRightTopAnchor{1.0f, 0.0f};
constexpr Vector4 kActiveColor{1.0f, 1.0f, 1.0f, 1.0f};
constexpr Vector4 kInactiveColor{0.65f, 0.65f, 0.65f, 0.78f};
} // namespace

TeamUI::TeamUI() { displayedCharacterIndices_.fill(-1); }

TeamUI::~TeamUI() = default;

void TeamUI::Initialize(const Team& team) {
	uint32_t hpBarTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBar.png");
	uint32_t hpBarBackgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Team/UI/HPBarBackground.png");
	uint32_t specialGaugeTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/specialGauge.png");
	uint32_t specialGaugeFlameTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/special.png");
	for (int i = 0; i < kMaxMembersCount; ++i) {
		iconPositions_[i] = {
		    static_cast<float>(WinApp::kClientWidth) - kRightMargin,
		    kTopMargin + (kIconSize.y + kIconSpacing) * static_cast<float>(i),
		};

		const int characterIndex = team.GetMemberCharacterIndex(i);
		displayedCharacterIndices_[i] = characterIndex;
		if (!team.GetHasMember(i) || characterIndex < 0) {
			iconSprites_[i].reset();
			hpBarSprites_[i].reset();
			hpBarBackgroundSprites_[i].reset();
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

		hpBarSprites_[i] = std::make_unique<Sprite>();
		hpBarSprites_[i]->Initialize(hpBarTextureHandle);
		hpBarSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		hpBarSprites_[i]->SetPosition(iconPositions_[i]+kHpBarOffset);
		hpBarSprites_[i]->SetScale(kHpBarMaxSize);
		hpBarSprites_[i]->Update();
		
		hpBarBackgroundSprites_[i] = std::make_unique<Sprite>();
		hpBarBackgroundSprites_[i]->Initialize(hpBarBackgroundTextureHandle);
		hpBarBackgroundSprites_[i]->SetAnchorPoint({1.0f,0.5f});
		hpBarBackgroundSprites_[i]->SetPosition(iconPositions_[i]+kHpBarOffset);
		hpBarBackgroundSprites_[i]->SetScale(kHpBarMaxSize);
		hpBarBackgroundSprites_[i]->Update();

		specialGaugeFlameSprites_[i] = std::make_unique<Sprite>();
		specialGaugeFlameSprites_[i]->Initialize(specialGaugeFlameTextureHandle);
		specialGaugeFlameSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		specialGaugeFlameSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset + Vector2{-kHpBarMaxSize.x*1.2f,-kHpBarOffset.y*0.4f});
		specialGaugeFlameSprites_[i]->SetScale(kIconSize);
		specialGaugeFlameSprites_[i]->Update();

		specialGaugeSprites_[i] = std::make_unique<Sprite>();
		specialGaugeSprites_[i]->Initialize(specialGaugeTextureHandle);
		specialGaugeSprites_[i]->SetAnchorPoint({1.0f, 0.5f});
		specialGaugeSprites_[i]->SetPosition(iconPositions_[i] + kHpBarOffset + Vector2{-kHpBarMaxSize.x * 1.2f, -kHpBarOffset.y*0.4f});
		specialGaugeSprites_[i]->SetScale(kIconSize);
		specialGaugeSprites_[i]->Update();


	}
}

void TeamUI::Update(const Team& team) {
	for (int i = 0; i < kMaxMembersCount; ++i) {
		const int characterIndex = team.GetMemberCharacterIndex(i);
		if (!team.GetHasMember(i) || characterIndex < 0) {
			iconSprites_[i].reset();
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

		displayedCharacterIndices_[i] = characterIndex;
		iconSprites_[i]->SetPosition(iconPositions_[i]);
		iconSprites_[i]->SetScale(i == team.GetActiveSlot() ? kActiveIconSize : kIconSize);
		iconSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		iconSprites_[i]->Update();
	}
}

void TeamUI::Draw() {
	for (auto& iconSprite : iconSprites_) {
		if (iconSprite) {
			iconSprite->Draw();
		}
	}
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
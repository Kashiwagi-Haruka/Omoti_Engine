#include "TeamUI.h"
#include "WinApp.h"

namespace {
constexpr Vector2 kIconSize{64.0f, 64.0f};
constexpr Vector2 kActiveIconSize{78.0f, 78.0f};
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
	for (int i = 0; i < kMaxMembersCount; ++i) {
		iconPositions_[i] = {
		    static_cast<float>(WinApp::kClientWidth) - kRightMargin,
		    kTopMargin + (kIconSize.y + kIconSpacing) * static_cast<float>(i),
		};

		const int characterIndex = team.GetMemberCharacterIndex(i);
		displayedCharacterIndices_[i] = characterIndex;
		if (!team.GetHasMember(i) || characterIndex < 0) {
			iconSprites_[i].reset();
			continue;
		}

		iconSprites_[i] = std::make_unique<Sprite>();
		iconSprites_[i]->Initialize(team.GetOwnedCharacterIconHandle(characterIndex));
		iconSprites_[i]->SetAnchorPoint(kRightTopAnchor);
		iconSprites_[i]->SetPosition(iconPositions_[i]);
		iconSprites_[i]->SetScale(i == team.GetActiveSlot() ? kActiveIconSize : kIconSize);
		iconSprites_[i]->SetColor(i == team.GetActiveSlot() ? kActiveColor : kInactiveColor);
		iconSprites_[i]->Update();
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
}
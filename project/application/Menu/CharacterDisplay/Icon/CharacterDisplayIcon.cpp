#define NOMINMAX
#include "CharacterDisplayIcon.h"
#include "Team/Team.h"
#include "TextureManager.h"
#include <algorithm>

namespace {
constexpr Vector2 kIconAnchor{0.5f, 0.5f};
constexpr Vector4 kSelectedIconColor{1.0f, 1.0f, 1.0f, 1.0f};
constexpr Vector4 kUnselectedIconColor{0.55f, 0.55f, 0.55f, 0.75f};
constexpr Vector4 kSelectedFrameColor{0.95f, 0.82f, 0.25f, 0.95f};
constexpr Vector4 kUnselectedFrameColor{0.1f, 0.1f, 0.12f, 0.6f};
constexpr Vector2 kFramePadding{10.0f, 10.0f};
} // namespace

void CharacterDisplayIcon::Initialize(const Team& team) {
	whiteTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	const int characterCount = std::max(0, team.GetOwnedCharacterCount());
	characterIcon_.clear();
	flameSprites_.clear();
	characterIcon_.reserve(characterCount);
	flameSprites_.reserve(characterCount);

	for (int i = 0; i < characterCount; ++i) {
		auto frameSprite = std::make_unique<Sprite>();
		frameSprite->Initialize(whiteTextureHandle_);
		frameSprite->SetAnchorPoint(kIconAnchor);
		frameSprite->SetPosition(CalculateIconPosition(static_cast<size_t>(i), static_cast<size_t>(characterCount)));
		frameSprite->SetScale({iconSize_.x + kFramePadding.x, iconSize_.y + kFramePadding.y});
		frameSprite->SetColor(kUnselectedFrameColor);
		frameSprite->Update();
		flameSprites_.push_back(std::move(frameSprite));

		auto iconSprite = std::make_unique<Sprite>();
		iconSprite->Initialize(team.GetOwnedCharacterIconHandle(i));
		iconSprite->SetAnchorPoint(kIconAnchor);
		iconSprite->SetPosition(CalculateIconPosition(static_cast<size_t>(i), static_cast<size_t>(characterCount)));
		iconSprite->SetScale(iconSize_);
		iconSprite->SetColor(kUnselectedIconColor);
		iconSprite->Update();
		characterIcon_.push_back(std::move(iconSprite));
	}
}

Vector2 CharacterDisplayIcon::CalculateIconPosition(size_t iconIndex, size_t iconCount) const {
	if (iconCount == 0) {
		return basePosition_;
	}
	const float interval = iconSize_.x + iconSpacing_;
	const float startX = basePosition_.x - (interval * static_cast<float>(iconCount - 1) * 0.5f);
	return {startX + interval * static_cast<float>(iconIndex), basePosition_.y};
}

void CharacterDisplayIcon::Update(size_t selectedCharacterIndex) {
	const size_t iconCount = characterIcon_.size();
	for (size_t i = 0; i < iconCount; ++i) {
		const bool isSelected = i == selectedCharacterIndex;
		const Vector2 iconPosition = CalculateIconPosition(i, iconCount);
		if (flameSprites_[i]) {
			flameSprites_[i]->SetPosition(iconPosition);
			const Vector2 frameSize = isSelected ? selectedIconSize_ : iconSize_;
			flameSprites_[i]->SetScale({frameSize.x + kFramePadding.x, frameSize.y + kFramePadding.y});
			flameSprites_[i]->SetColor(isSelected ? kSelectedFrameColor : kUnselectedFrameColor);
			flameSprites_[i]->Update();
		}
		if (characterIcon_[i]) {
			characterIcon_[i]->SetPosition(iconPosition);
			characterIcon_[i]->SetScale(isSelected ? selectedIconSize_ : iconSize_);
			characterIcon_[i]->SetColor(isSelected ? kSelectedIconColor : kUnselectedIconColor);
			characterIcon_[i]->Update();
		}
	}
}

void CharacterDisplayIcon::Draw() {
	for (auto& frameSprite : flameSprites_) {
		if (frameSprite) {
			frameSprite->Draw();
		}
	}
	for (auto& iconSprite : characterIcon_) {
		if (iconSprite) {
			iconSprite->Draw();
		}
	}
}
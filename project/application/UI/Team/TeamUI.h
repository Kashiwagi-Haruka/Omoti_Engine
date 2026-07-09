#pragma once
#include "Sprite/Sprite.h"
#include "Team/Team.h"
#include "Text/Text.h"
#include "Vector2.h"
#include <array>
#include <cstdint>
#include <memory>

class TeamUI {
public:
	TeamUI();
	~TeamUI();
	void Initialize(const Team& team);
	void Update(const Team& team);
	void Draw();

private:

	void UpdateInputDisplayMode();

	enum class InputDisplayMode {
		Keyboard,
		Pad,
	};

	static constexpr int kMaxMembersCount = Team::kMaxMembersCount;
	InputDisplayMode inputDisplayMode_ = InputDisplayMode::Keyboard;

	std::unique_ptr<Sprite> selectedBackgroundSprite_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> iconSprites_;
	std::array<Vector2, kMaxMembersCount> iconPositions_{};
	std::array<int, kMaxMembersCount> displayedCharacterIndices_{};
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarBackgroundSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> specialGaugeSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> specialGaugeFlameSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> padSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> keyboardSprites_;
	uint32_t characterNameFontHandle_ = 0;
	std::array<Text, kMaxMembersCount> characterNameTexts_;
};
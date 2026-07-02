#pragma once
#include "Sprite/Sprite.h"
#include "Team/Team.h"
#include "Vector2.h"
#include <array>
#include <memory>
#include "Text/Text.h"

class TeamUI {
public:
	TeamUI();
	~TeamUI();
	void Initialize(const Team& team);
	void Update(const Team& team);
	void Draw();

private:
	static constexpr int kMaxMembersCount = Team::kMaxMembersCount;

	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> selectedBackgroundSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> iconSprites_;
	std::array<Vector2, kMaxMembersCount> iconPositions_{};
	std::array<int, kMaxMembersCount> displayedCharacterIndices_{};
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarBackgroundSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> specialGaugeSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> specialGaugeFlameSprites_;
	std::array<Text, kMaxMembersCount> CharacterNameTexts_;
};
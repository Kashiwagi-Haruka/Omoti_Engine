#pragma once
#include "Sprite/Sprite.h"
#include "Team/Team.h"
#include "Vector2.h"
#include <array>
#include <memory>

class TeamUI {
public:
	TeamUI();
	~TeamUI();
	void Initialize(const Team& team);
	void Update(const Team& team);
	void Draw();

private:
	static constexpr int kMaxMembersCount = Team::kMaxMembersCount;

	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> iconSprites_;
	std::array<Vector2, kMaxMembersCount> iconPositions_{};
	std::array<int, kMaxMembersCount> displayedCharacterIndices_{};
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarSprites_;
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> hpBarBackgroundSprites_;
};
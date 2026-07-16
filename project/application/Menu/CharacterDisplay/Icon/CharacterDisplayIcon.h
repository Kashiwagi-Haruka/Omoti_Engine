#pragma once
#include "Sprite.h"
#include "Vector2.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class Team;

class CharacterDisplayIcon {
	// キャラクターアイコンスプライト
	std::vector<std::unique_ptr<Sprite>> characterIcon_;
	// チームフレームスプライト
	std::vector<std::unique_ptr<Sprite>> flameSprites_;

	Vector2 basePosition_{640.0f, 190.0f};
	Vector2 iconSize_{56.0f, 56.0f};
	Vector2 selectedIconSize_{66.0f, 66.0f};
	float iconSpacing_ = 18.0f;
	uint32_t whiteTextureHandle_ = 0;

	Vector2 CalculateIconPosition(size_t iconIndex, size_t iconCount) const;

public:
	CharacterDisplayIcon() = default;
	~CharacterDisplayIcon() = default;
	void Initialize(const Team& team);
	void Update(size_t selectedCharacterIndex);
	void Draw();
};
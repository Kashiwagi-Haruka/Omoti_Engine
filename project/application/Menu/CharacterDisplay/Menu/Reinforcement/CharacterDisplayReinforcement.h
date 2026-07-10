#pragma once
#include "Sprite.h"
#include <array>
#include <memory>
class CharacterDisplayReinforcement {

	// 最大分割数
	static const uint32_t kMaxSprit_ = 6;

	// 背景スプライト
	std::unique_ptr<Sprite> backgroundSprite_ = nullptr;
	// 全体絵スプライト
	std::unique_ptr<Sprite> fullSprite_ = nullptr;
	// 分割スプライト
	std::array<std::unique_ptr<Sprite>, kMaxSprit_> spritSprites_;

public:
	void Initialize();
	void Update();
	void Draw();
};

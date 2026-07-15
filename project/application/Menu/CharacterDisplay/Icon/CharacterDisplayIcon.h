#pragma once
#include "Sprite.h"

#include <vector>
#include <memory>
class CharacterDisplayIcon {
	// キャラクターアイコンスプライト
	std::vector<std::unique_ptr<Sprite>> characterIcon_;
	// チームフレームスプライト
	std::vector<std::unique_ptr<Sprite>> flameSprites_;

	public:

	CharacterDisplayIcon() = default;
	~CharacterDisplayIcon() = default;
	void Initialize();
	void Update();
	void Draw();
	void SetTeam();
};

#pragma once
#include "Sprite/Sprite.h"
#include <memory>
#include <array>
class TeamUI {

	std::array<std::unique_ptr<Sprite>, 4> iconSprite_;
	std::array<Vector2, 3> iconPos_;
	std::array<uint32_t, 4> teamMemberIndex_;


public:
	void Initialize();
	void Update();
	void Draw();
	void ChangeMenber(uint32_t prePlayerMemberIndex,uint32_t playerMemberIndex);
};

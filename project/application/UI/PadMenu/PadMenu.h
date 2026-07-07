#pragma once
#include "Sprite.h"
#include <memory>
class PadMenu {

	std::unique_ptr<Sprite> flameSprite_;
	std::unique_ptr<Sprite> selectSprite_;
	std::unique_ptr<Sprite> backgroundSprite_;

	float selectRotation_ = 0.0f;

public:
	PadMenu();
	~PadMenu();
	void Initialize();
	void Update();
	void Draw();

};

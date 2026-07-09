#pragma once
#include "Sprite.h"
#include <memory>
class PadMenu {

	std::unique_ptr<Sprite> flameSprite_;
	std::unique_ptr<Sprite> selectSprite_;
	std::unique_ptr<Sprite> backgroundSprite_;

	#pragma region ShortCutMenu
	std::unique_ptr<Sprite> teamDisplayIcon_;
	std::unique_ptr<Sprite> characterDisplayIcon_;
	std::unique_ptr<Sprite> HintIcon_;
	
	#pragma endregion

	float selectRotation_ = 0.0f;
	bool isMenuSelect = false;

public:
	PadMenu();
	~PadMenu();
	void Initialize();
	void Update();
	void Draw();

};

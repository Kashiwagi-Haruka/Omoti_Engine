#pragma once
#include "Text/Text.h"
class CharacterDisplayStatus {
	std::string name_;

	Text nameText_;
	Text levelText_;
	Text expText_;
	Text statusText_;

public:
	void Initialize();
	void Update();
	void Draw();
};

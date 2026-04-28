#pragma once
#include "Text/Text.h"
#include <string>
class CharacterDisplayText {

	std::string characterName_;


	Text CharacterNameText_;
	Text CharacterLevelText_;

	public: 

		CharacterDisplayText();
	    void Initialize();
	    void Update();
	    void Draw();
	    void SetCharacterName(const std::u32string& name);
};

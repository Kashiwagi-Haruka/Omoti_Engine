#pragma once
#include "Text/Text.h"
class QuestUI {

	Text QuestText;

	public: 

	void Initialize();
	void Update();
	void Draw();
	void QuestChange(std::string QuestName);

};

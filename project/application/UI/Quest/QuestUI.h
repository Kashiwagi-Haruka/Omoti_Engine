#pragma once
#include "Text/Text.h"
#include "Sprite.h"
#include <memory>
class QuestUI {

	Text QuestText;
	std::unique_ptr<Sprite> questBGSprite_;

	public: 

	QuestUI();
	~QuestUI();
	void Initialize();
	void Update();
	void Draw();
	void QuestChange(std::string QuestName);

};

#pragma once
#include "Sprite.h"
#include <memory>
#include <map>
class HelpDisplay {
private:

	struct HelpData {
		std::unique_ptr<Sprite> sprite = nullptr;
		std::string mainText = "";
	};


	std::unique_ptr<Sprite> backgroundSprite_;
	std::map<std::string, HelpData> helpData_;

public:
	HelpDisplay();
	~HelpDisplay() = default;
	void Initialize();
	void Update();
	void Draw();
	void Finalize();
	void Open();
};

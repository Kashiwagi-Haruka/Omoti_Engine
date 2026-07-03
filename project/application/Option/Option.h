#pragma once
#include "Text/Text.h"
#include <vector>
class Option {

	enum class Menu {
		kCAMERA,
		kSOUND,
		kBACK,
	};

	std::vector<Text> menuTexts_;	

public:
	Option();
	~Option();
	void Initialize();
	void Update();
	void Draw();

};

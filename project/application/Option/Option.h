#pragma once
#include "Text/Text.h"
#include <vector>
class Option {

	enum class Menu {
		// カメラ
		kCAMERA,
		// 音
		kSOUND,
		// 表示
		kVIEW,
		// 戻る
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

#pragma once
#include "Vector2.h"
class DashGauge {

	Vector2 position_{};
	Vector2 size_{};


public:
	void Initialize();
	void Update();
	void Draw();
};

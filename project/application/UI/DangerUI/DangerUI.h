#pragma once
#include "Sprite.h"
#include <vector>
#include <memory>
class DangerUI {

	enum class DangerState { kNone, kStart,kActive, kEnd };

	DangerState dangerState_ = DangerState::kNone;
	std::vector<std::unique_ptr<Sprite>> dangerSprite_;
	float dangerTimer_ = 0.0f;	


	public:

	DangerUI();
	~DangerUI();
	void Initialize();
	void Update();
	void Draw();
	void Start();
	void End();
};

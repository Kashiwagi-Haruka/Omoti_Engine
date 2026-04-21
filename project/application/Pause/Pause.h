#pragma once
#include "Sprite.h"
#include "Vector2.h"
#include <array>
#include <memory>
#include "Mesh/Object3d/Object3d.h"

class Pause {

public:
	enum class Action {
		kNone,
		kResume,
		kTitle,
	};

private:
	std::unique_ptr<Sprite> Select_;
	std::unique_ptr<Sprite> Button_;
	std::unique_ptr<Object3d> BackGround_;
	Object3d* currentCharacterObj_ = nullptr;

	uint32_t BGHandle_;
	uint32_t SelectHandle_;
	uint32_t ButtonHandle_;

	Vector2 bgBasePos_{0.0f, 0.0f};
	Vector2 selectBasePos_{0.0f, 0.0f};
	std::array<Vector2, 2> buttonBasePos_{};

	Vector2 bgSize_{1280.0f, 720.0f};
	Vector2 selectSize_{400.0f, 400.0f};
	Vector2 buttonSize_{80.0f, 80.0f};

	float startTime = 0.0f;
	int selectIndex_ = 1;

	bool isActive_ = false;
	bool isStart_ = false;
	bool isEnd_ = false;

	Action action_ = Action::kNone;

public:
	Pause();
	void Initialize();
	void Update(bool isPause);
	void Draw();
	bool IsVisible() const { return isActive_ || isStart_ || isEnd_; }
	void SetCurrentCharacterObj(Object3d* obj) { currentCharacterObj_ = obj; }
	Action ConsumeAction();
};
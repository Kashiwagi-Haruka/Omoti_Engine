#pragma once
#include "Mesh/Object3d/Object3d.h"
#include "Mesh/Primitive/Primitive.h"
#include "Sprite.h"
#include "Text/Text.h"
#include "Vector2.h"
#include <array>
#include <memory>
#include "Camera.h"
#include "Object/Characters/Base/Attribute.h"

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
	std::unique_ptr<Primitive> BG_;
	Object3d* currentCharacterObj_ = nullptr;

	uint32_t SelectHandle_;
	uint32_t ButtonHandle_;

	Vector2 selectBasePos_{0.0f, 0.0f};
	std::array<Vector2, 2> buttonBasePos_{};
	std::array<std::unique_ptr<Sprite>, 3> selectSprites_{};

	Vector2 selectSize_{400.0f, 400.0f};
	Vector2 buttonSize_{80.0f, 80.0f};

	float startTime = 0.0f;
	int selectIndex_ = 1;

	bool isActive_ = false;
	bool isStart_ = false;
	bool isEnd_ = false;

	Action action_ = Action::kNone;
	std::unique_ptr<Camera> camera_;
	Text pauseText_;
	std::vector<Text> menuTexts_;
	uint32_t pauseFontHandle_ = 0;
	Attribute currentAttribute = Attribute::None;

public:
	Pause();
	void Initialize();
	void Update(bool isPause);
	void Draw();
	bool IsVisible() const { return isActive_ || isStart_ || isEnd_; }
	void SetCurrentCharacterObj(Object3d* obj) { currentCharacterObj_ = obj; }
	void SetCurrentAttribute(Attribute attribute) { currentAttribute = attribute; }
	Action ConsumeAction();
};
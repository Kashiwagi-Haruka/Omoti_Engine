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
#include "Option/Option.h"

class Pause {

public:
	enum class Action {
		kNone,
		kResume,
		kOptions,
		kLicense,
		kTitle,
	};
	enum class PauseState {
		kNone,
		kStart,
		kActive,
		kEnd,
	};

private:

	std::unique_ptr<Primitive> BG_;
	Object3d* currentCharacterObj_ = nullptr;

	std::array<std::unique_ptr<Sprite>, 3> selectSprites_{};
	float selectRotation_[3]{};
	float selectScale_[3]{};

	float startTime = 0.0f;
	int selectIndex_ = 1;

	//　ポーズ画面中か
	bool isPause_;
	// ポーズ画面の状態
	PauseState pauseState_ = PauseState::kNone;
	// タイトルに戻るか
	bool isTitleBack_ = false;
	//　タイトル画面に戻すか聞くスプライトを表示するか
	bool isTitleBackVisit_ = false;

	Action action_ = Action::kNone;
	std::unique_ptr<Camera> camera_;
	Text pauseText_;
	std::vector<Text> menuTexts_;
	uint32_t pauseFontHandle_ = 0;

	std::unique_ptr<Option> option_;
	Attribute currentAttribute = Attribute::None;

	std::unique_ptr<Sprite> titleBackBG_;
	Text titleBackText_;

	void CurrentCharacterUpdate();
	void SelectSpriteUpdate();

public:
	Pause();
	void Initialize();
	void Start();
	void End();
	void Update();
	void Draw();
	void SetCurrentCharacterObj(Object3d* obj) { currentCharacterObj_ = obj; }
	void SetCurrentAttribute(Attribute attribute) { currentAttribute = attribute; }
	bool IsPause() const { return isPause_; };
	bool IsTitleBack() const;

};
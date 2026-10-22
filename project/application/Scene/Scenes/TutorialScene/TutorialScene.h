#pragma once

#include "BaseScene.h"
#include "Object/Characters/Base/Attribute.h"
#include "Text/Text.h"
#include <array>
#include <memory>
#include <string>

class CameraController;
class Field;
class Player;
class Sky;
class Sprite;
class TutorialUI;

class TutorialScene : public BaseScene {
public:
	TutorialScene();
	~TutorialScene() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	enum class TutorialStep { kMove = 0, kLook, kAttack, kSkill, kSpecial, kAttributeChange, kPause, kComplete };

	static constexpr int kStepCount = static_cast<int>(TutorialStep::kComplete) + 1;

	static constexpr int kSkillUseTargetCount = 3;
	static constexpr int kAttributeCount = 6;

	static constexpr float kSkipHoldDuration = 1.0f;
	static constexpr float kMoveActionDuration = 2.0f;
	static constexpr float kLookActionDuration = 2.0f;
	static constexpr float kCompleteDuration = 2.0f;

	TutorialStep GetCurrentStep() const;

	void BeginStep();
	void AdvanceStep();
	void UpdateTutorial(float deltaTime, bool skillStarted, bool specialStarted);

	void InitializeAttributeMenu();
	void UpdateAttributeMenu();
	void UpdateGuide();

	std::unique_ptr<CameraController> cameraController_;
	std::unique_ptr<Sky> skyDome_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Field> field_;
	std::unique_ptr<Sprite> controlSprite_;
	std::unique_ptr<TutorialUI> tutorialUI_;

	std::array<std::unique_ptr<Sprite>, kAttributeCount> attributeSprites_;

	Text guideText_;
	std::u32string displayedGuide_;

	int currentStepIndex_ = 0;
	int skillUseCount_ = 0;
	int selectedAttributeIndex_ = 1;

	bool isTutorialComplete_ = false;
	bool isPaused_ = false;
	bool wasSkipKeyHeld_ = false;
	bool pauseOpened_ = false;
	bool isAttributeMenuOpen_ = false;

	float skipHoldTimer_ = 0.0f;
	float stepTimer_ = 0.0f;
	float stepActionTimer_ = 0.0f;
	float currentStepProgress_ = 0.0f;

	Attribute stepStartAttribute_ = Attribute::None;
};
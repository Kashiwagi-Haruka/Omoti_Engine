#pragma once

#include "BaseScene.h"
#include "Text/Text.h"
#include "Scene/Scenes/TutorialScene/TutorialFlow/TutorialFlow.h"

#include <array>
#include <memory>
#include <string>

class CameraController;
class Field;
class Player;
class Sky;
class Sprite;
class TutorialUI;
class UIManager;

class TutorialScene : public BaseScene {
public:
	TutorialScene();
	~TutorialScene() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	static constexpr int kAttributeCount = 6;
	static constexpr float kSkipHoldDuration = 1.0f;

	void InitializeAttributeMenu();
	void UpdateAttributeMenu();
	void UpdateGuide(const TutorialFrame& frame);

	TutorialFlow tutorialFlow_;

	std::unique_ptr<CameraController> cameraController_;
	std::unique_ptr<Sky> skyDome_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Field> field_;
	std::unique_ptr<Sprite> controlSprite_;
	std::unique_ptr<TutorialUI> tutorialUI_;
	std::unique_ptr<UIManager> uiManager_;
	std::unique_ptr<Sprite> guideWindowSprite_;

	std::array<std::unique_ptr<Sprite>, kAttributeCount> attributeSprites_;

	Text guideText_;
	std::u32string displayedGuide_;

	int selectedAttributeIndex_ = 1;
	bool isPaused_ = false;
	bool wasSkipKeyHeld_ = false;
	bool isAttributeMenuOpen_ = false;
	float skipHoldTimer_ = 0.0f;
};
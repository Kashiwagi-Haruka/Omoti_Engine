#pragma once

#include "BaseScene.h"
#include "Scene/Scenes/TutorialScene/TutorialFlow/TutorialFlow.h"
#include "Scene/Scenes/TutorialScene/TutorialProgressWindow/TutorialProgressWindow.h"
#include "Text/Text.h"

#include <array>
#include <memory>
#include <optional>
#include <string>

class CameraController;
class Damage;
class Field;
class Object3d;
class Player;
class Sky;
class Sprite;
class TutorialUI;
class UIManager;
class Adhesion;

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
	std::optional<int> FindHoveredAttributeIndex() const;
	bool UpdateAttributeMenu(bool wasOpen);
	void UpdateTrainingDummy(bool normalAttackStarted);
	void ShowTrainingHit();
	void UpdateGuide(const TutorialFrame& frame);

	TutorialFlow tutorialFlow_;
	TutorialProgressWindow progressWindow_;

	std::unique_ptr<CameraController> cameraController_;
	std::unique_ptr<Sky> skyDome_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Field> field_;
	std::unique_ptr<TutorialUI> tutorialUI_;
	std::unique_ptr<UIManager> uiManager_;
	std::unique_ptr<Sprite> guideWindowSprite_;
	std::unique_ptr<Object3d> trainingDummy_;
	std::unique_ptr<Damage> trainingDamage_;
	std::array<std::unique_ptr<Sprite>, kAttributeCount> attributeSprites_;
	std::unique_ptr<Adhesion> trainingAdhesion_;
	Text guideText_;
	std::u32string displayedGuide_;
	std::u32string trainingHitText_;

	int activeSwordComboStep_ = 0;
	bool swordAttackHit_ = false;
	std::optional<int> lastSkillDamageId_;
	std::optional<int> lastSpecialDamageId_;
	std::optional<int> lastRainDamageId_;

	bool isPaused_ = false;
	bool wasSkipKeyHeld_ = false;
	bool isAttributeMenuOpen_ = false;
	float skipHoldTimer_ = 0.0f;
};
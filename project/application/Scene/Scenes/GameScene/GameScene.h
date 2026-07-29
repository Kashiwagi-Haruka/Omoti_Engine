#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "CollisionManager/CollisionManager.h"
#include "GameBase.h"
#include "Light/CommonLight/DirectionalCommonLight.h"
#include "Light/CommonLight/PointCommonLight.h"
#include "Light/CommonLight/SpotCommonLight.h"
#include "Menu/TeamDisplay/TeamDisplay.h"
#include "Object/Characters/Model/CharacterModel.h"
#include "Object/Field/Field.h"
#include "Object/RemoteCamera/RemoteCamera.h"
#include "Object3d/Object3d.h"
#include "OpenWorld/OpenWorld.h"
#include "Rasen/Rasen.h"
#include "SceneTransition/SceneTransition.h"
#include "Sprite.h"
#include "Team/Team.h"
#include "UI/UIManager/UIManager.h"
#include "Vector2.h"
#include "Menu/MenuManager.h"
#include <array>
#include <cstdint>
#include <imgui.h>
#include <string>
class Player;
class CameraController;
class Sky;
class Boss;
class Enemy;
class Camera;
class SpecialGaugeBallManager;

class GameScene : public BaseScene {

private:
	bool IsPKey = false;
	bool IsXButton = false;
	bool IsKeyboard = true;

	uint32_t color;

	std::unique_ptr<UIManager> uimanager;
	std::unique_ptr<SceneTransition> sceneTransition;
	std::unique_ptr<Player> player;
	std::unique_ptr<Sky> skyDome;
	std::unique_ptr<CameraController> cameraController;
	std::unique_ptr<Field> field;
	std::unique_ptr<RemoteCamera> remoteCamera_;
	std::unique_ptr<MenuManager> menuManager_;

	CollisionManager collisionManager_;

	bool isTransitionIn = true;
	bool isTransitionOut = false;
	std::string nextSceneName;
	bool isPause = false;
	bool isCharacterDisplayMode_ = false;
	bool isPartyMode_ = false;

	CharacterModel characterModel;
	std::unique_ptr<Team> team_;
	std::unique_ptr<TeamDisplay> teamDisplay_;

	enum class PlayAreaMode { kSpiral, kOpenWorld };

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<Rasen> rasen_;
	std::unique_ptr<OpenWorld> openWorld_;
	std::unique_ptr<SpecialGaugeBallManager> specialGaugeBallManager_;
	PlayAreaMode playAreaMode_ = PlayAreaMode::kSpiral;

	DirectionalCommonLight directionalLight_{};
	std::array<PointCommonLight, kMaxPointLights> pointLights_{};
	uint32_t activePointLightCount_ = 0;
	std::array<SpotCommonLight, kMaxSpotLights> spotLights_{};
	uint32_t activeSpotLightCount_ = 0;

	bool sceneEndClear = false;
	bool sceneEndOver = false;
	float damageGrayscaleTimer_ = 0.0f;
	static constexpr float kDamageGrayscaleDuration_ = 0.5f;

		std::unique_ptr<Sprite> hitVinettSprite_;
	uint32_t hitVinettTextureHandle_ = 0;
	float hitVinettTimer_ = 0.0f;
	static constexpr float kHitVinettDuration_ = 0.25f;

	Vector3 vinettColor_;
	float vinettStrength_;
	float introBlurDelayTimer_ = 0.0f;
	static constexpr float kIntroBlurDelaySeconds_ = 2.0f;
	static constexpr float kIntroBlurFadeDurationSeconds_ = 1.0f;
	float introBlurStartKernelSize_ = 1.0f;
	int fullscreenFilterType_ = 1;
	float gaussianFilterSigma_ = 2.0f;
	Vector2 radialBlurCenter_ = {0.5f, 0.5f};
	float radialBlurWidth_ = 0.01f;
	int radialBlurSampleCount_ = 10;
	bool chromaticAberrationEnabled_ = false;
	float chromaticAberrationIntensity_ = 0.02f;
	bool dissolveEnabled_ = false;
	float dissolveThreshold_ = 0.0f;
	float dissolveEdgeWidth_ = 0.02f;
	Vector4 dissolveEdgeColor_ = {0.35f, 0.95f, 1.0f, 1.0f};
	Vector4 characterDeathDissolveStartColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
	bool isCharacterDeathDissolving_ = false;
	float characterDeathDissolveTimer_ = 0.0f;
	static constexpr float kCharacterDeathDissolveDuration_ = 1.0f;
	Enemy* lockOnMarkerEnemy_ = nullptr;
	Enemy* normalAttackTargetEnemy_ = nullptr;
	float lockOnMarkerTimer_ = 0.0f;
	static constexpr float kLockOnMarkerDuration_ = 0.5f;
	bool isGameOverPending_ = false;
	float gameOverDelayTimer_ = 0.0f;
	static constexpr float kGameOverDelaySeconds_ = 1.0f;

	void DrawRemoteCameraScene(Camera* camera);

public:
	GameScene();
	~GameScene() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void DebugImGui();
	void LoadTeamDisplay();
	void UnloadTeamDisplay();
};
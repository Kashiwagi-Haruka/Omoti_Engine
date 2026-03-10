#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "CollisionManager/CollisionManager.h"
#include "GameBase.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"
#include "Object/Character/CharacterDisplay/CharacterDisplay.h"
#include "Object/Character/Model/CharacterModel.h"
#include "Object/MapchipField.h"
#include "Object3d/Object3d.h"
#include "OpenWorld/OpenWorld.h"
#include "Particles/Particles.h"
#include "Pause/Pause.h"
#include "Rasen/Rasen.h"
#include "SceneTransition/SceneTransition.h"
#include "Sprite.h"
#include "UIManager/UIManager.h"
#include "Vector2.h"
#include <array>
#include <cstdint>
#include <imgui.h>
#include <string>
class Player;
class CameraController;
class SkyDome;
class Boss;

class GameScene : public BaseScene {

private:
	bool IsPKey = false;
	bool IsXButton = false;
	bool IsKeyboard = true;

	uint32_t color;
	std::unique_ptr<Particles> particles;
	std::unique_ptr<UIManager> uimanager;
	std::unique_ptr<SceneTransition> sceneTransition;
	std::unique_ptr<Player> player;
	std::unique_ptr<SkyDome> skyDome;
	std::unique_ptr<CameraController> cameraController;
	std::unique_ptr<MapchipField> field;
	std::unique_ptr<Pause> pause;

	CollisionManager collisionManager_;

	bool isTransitionIn = true;
	bool isTransitionOut = false;
	std::string nextSceneName;
	bool isBGMPlaying = false;
	bool isPause = false;
	bool isCharacterDisplayMode_ = false;

	SoundData BGMData;

	CharacterModel characterModel;
	std::unique_ptr<CharacterDisplay> characterDisplay_;

	enum class PlayAreaMode { kSpiral, kOpenWorld };

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<Rasen> rasen_;
	std::unique_ptr<OpenWorld> openWorld_;
	PlayAreaMode playAreaMode_ = PlayAreaMode::kSpiral;

	DirectionalLight directionalLight_{};
	std::array<PointLight, kMaxPointLights> pointLights_{};
	uint32_t activePointLightCount_ = 0;
	std::array<SpotLight, kMaxSpotLights> spotLights_{};
	uint32_t activeSpotLightCount_ = 0;

	bool sceneEndClear = false;
	bool sceneEndOver = false;

public:
	GameScene();
	~GameScene() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void DebugImGui();
};
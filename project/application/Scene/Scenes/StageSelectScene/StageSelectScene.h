#pragma once
#include "BaseScene.h"
#include "Object/StageSelect/Gate/StageSelectGate.h"
#include "Object/Player/Player.h"
#include "CameraController/CameraController.h"
#include "Mesh/SkyBox/SkyBox.h"
#include <memory>

class StageSelectScene : public BaseScene {

	enum class StageNames {
		NONE,
		BACK,
		TUTORIAL_STAGE,
		STAGE1,
	};

	std::unique_ptr<StageSelectGate> tutorialStageGate_;
	std::unique_ptr<StageSelectGate> stage1Gate_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<CameraController> cameraController_;
	std::unique_ptr<SkyBox> skyBox_;

	
	StageSelectScene::StageNames selectedStage_ = StageSelectScene::StageNames::NONE;


	public:

	StageSelectScene();
	~StageSelectScene() override = default;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};

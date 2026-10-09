#pragma once
#include "BaseScene.h"
#include "Object3d/Object3d.h"
#include <memory>

class StageSelectScene : public BaseScene {

	enum class StageNames {
		NONE,
		BACK,
		TUTORIAL_STAGE,
		STAGE1,
	};

	std::unique_ptr<Object3d> tutorialStageGate_;
	std::unique_ptr<Object3d> stage1Gate_;


	StageSelectScene::StageNames selectedStage_ = StageSelectScene::StageNames::NONE;


	public:

	StageSelectScene();
	~StageSelectScene() override = default;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};

#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>

class StageSelectScene : public BaseScene {

	enum class StageNames {
		Back,
		TutorialStage,
		Stage1,
	};

	std::unique_ptr<Sprite> backgroundSP_;
	std::unique_ptr<Sprite> tutorialStageSP_;
	std::unique_ptr<Sprite> stage1SP_;


	


	public:

	StageSelectScene();
	~StageSelectScene() override = default;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};

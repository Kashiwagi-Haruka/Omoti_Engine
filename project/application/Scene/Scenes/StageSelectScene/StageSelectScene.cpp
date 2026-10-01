#include "StageSelectScene.h"
#include "TextureManager.h"
#include "SpriteCommon.h"

StageSelectScene::StageSelectScene() { 
	backgroundSP_ = std::make_unique<Sprite>();
	tutorialStageSP_ = std::make_unique<Sprite>();
	stage1SP_ = std::make_unique<Sprite>();
}

void StageSelectScene::Finalize(){};

void StageSelectScene::Initialize() {
	uint32_t handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/StageSelect/background.png");
	backgroundSP_->Initialize(handle);
	handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/StageSelect/tutorialStage.png");
	tutorialStageSP_->Initialize(handle);
	handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/StageSelect/stage1.png");
	stage1SP_->Initialize(handle);
	selectedStage_ = StageSelectScene::StageNames::TutorialStage;
}

void StageSelectScene::Update() { 
	backgroundSP_->Update();
	tutorialStageSP_->Update();
	stage1SP_->Update();
}

void StageSelectScene::Draw() { 
	SpriteCommon::GetInstance()->DrawCommon();
	backgroundSP_->Draw(); 
	tutorialStageSP_->Draw();
	stage1SP_->Draw();
}
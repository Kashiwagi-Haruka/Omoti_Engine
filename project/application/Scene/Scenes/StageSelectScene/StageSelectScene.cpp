#include "StageSelectScene.h"
#include "TextureManager.h"
#include "Model/ModelManager.h"

StageSelectScene::StageSelectScene() { 
	tutorialStageGate_ = std::make_unique<Object3d>();
	stage1Gate_ = std::make_unique<Object3d>();
}

void StageSelectScene::Finalize(){};

void StageSelectScene::Initialize() {

	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");

	tutorialStageGate_->SetModel("gate");
	tutorialStageGate_->Initialize();

	stage1Gate_->SetModel("gate");
	stage1Gate_->Initialize();

	selectedStage_ = StageSelectScene::StageNames::NONE;
}

void StageSelectScene::Update() { 

}

void StageSelectScene::Draw() { 

}
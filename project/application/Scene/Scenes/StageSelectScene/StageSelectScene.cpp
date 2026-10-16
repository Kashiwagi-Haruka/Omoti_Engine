#include "StageSelectScene.h"
#include "Object3d/Object3dCommon.h"
#include "GameBase.h"
#include "Model/ModelManager.h"

StageSelectScene::StageSelectScene() { 
	tutorialStageGate_ = std::make_unique<StageSelectGate>();
	stage1Gate_ = std::make_unique<StageSelectGate>();
	player_ = std::make_unique<Player>();
	cameraController_ = std::make_unique<CameraController>();
	skyBox_ = std::make_unique<SkyBox>();
}

void StageSelectScene::Finalize(){};

void StageSelectScene::Initialize() {
	
	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");
	cameraController_->Initialize();
	tutorialStageGate_->Initialize();
	stage1Gate_->Initialize();
	player_->Initialize(cameraController_->GetCamera());
	tutorialStageGate_->SetCamera(cameraController_->GetCamera());
	stage1Gate_->SetCamera(cameraController_->GetCamera());
	skyBox_->Initialize();
	skyBox_->SetDDSTexture("Resources/SkyBox/quantum_deepsea_skybox.dds");
	skyBox_->SetCamera(cameraController_->GetCamera());


	selectedStage_ = StageSelectScene::StageNames::NONE;
}

void StageSelectScene::Update() { 
	
	player_->SetCamera(cameraController_->GetCamera());
	tutorialStageGate_->SetCamera(cameraController_->GetCamera());
	stage1Gate_->SetCamera(cameraController_->GetCamera());
	skyBox_->SetCamera(cameraController_->GetCamera());

	player_->Update();
	tutorialStageGate_->Update();
	stage1Gate_->Update();
	skyBox_->Update();

	Transform cameraPlayerTransform = {player_->GetScale(), player_->GetRotate(), player_->GetPosition()};
	cameraController_->SetPlayerTransform(cameraPlayerTransform);
	cameraController_->Update();
}

void StageSelectScene::Draw() { 
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::Skybox);
	skyBox_->Draw();
	Object3dCommon::GetInstance()->DrawCommon();
	player_->Draw(false);
	tutorialStageGate_->Draw();
	stage1Gate_->Draw();
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningOutline);
	player_->Draw(true);
	Object3dCommon::GetInstance()->EndOutlineDraw();

}
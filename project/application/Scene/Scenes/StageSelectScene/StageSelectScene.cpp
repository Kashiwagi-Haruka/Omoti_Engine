#include "StageSelectScene.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "SceneManager.h"
#include <cmath>

StageSelectScene::StageSelectScene() {
	tutorialStageGate_ = std::make_unique<StageSelectGate>();
	stage1Gate_ = std::make_unique<StageSelectGate>();
	player_ = std::make_unique<Player>();
	cameraController_ = std::make_unique<CameraController>();
	skyBox_ = std::make_unique<SkyBox>();
	field_ = std::make_unique<Field>();
}

void StageSelectScene::Finalize() {}

void StageSelectScene::Initialize() {
	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");

	cameraController_->Initialize();

	tutorialStageGate_->Initialize();
	stage1Gate_->Initialize();

	player_->Initialize(cameraController_->GetCamera());

	// 仮配置。プレイヤーの初期位置は {-5, 3.6, -5}。
	tutorialStageGate_->SetPosition({-5.0f, 3.6f, 5.0f});
	stage1Gate_->SetPosition({5.0f, 3.6f, 5.0f});

	tutorialStageGate_->SetCamera(cameraController_->GetCamera());
	field_->Initialize(cameraController_->GetCamera());
	stage1Gate_->SetCamera(cameraController_->GetCamera());

	skyBox_->Initialize();
	skyBox_->SetDDSTexture("Resources/SkyBox/quantum_deepsea_skybox.dds");
	skyBox_->SetCamera(cameraController_->GetCamera());

	selectedStage_ = StageNames::NONE;
}

void StageSelectScene::Update() {
	// フェード中などにシーン切り替えを重複要求しない。
	if (selectedStage_ != StageNames::NONE) {
		return;
	}

	player_->SetCamera(cameraController_->GetCamera());
	tutorialStageGate_->SetCamera(cameraController_->GetCamera());
	stage1Gate_->SetCamera(cameraController_->GetCamera());
	skyBox_->SetCamera(cameraController_->GetCamera());
	field_->SetCamera(cameraController_->GetCamera());	

	player_->Update();
	tutorialStageGate_->Update();
	stage1Gate_->Update();
	skyBox_->Update();
	field_->Update();

	Transform cameraPlayerTransform = {player_->GetScale(), player_->GetRotate(), player_->GetPosition()};

	cameraController_->SetPlayerTransform(cameraPlayerTransform);
	cameraController_->Update();

	const Vector3 playerPosition = player_->GetPosition();
	const Vector3 playerScale = player_->GetScale();

	// プレイヤーの位置を中心とする仮の接触判定。
	// モデルの大きさに合わせて係数を調整する。
	const Vector3 playerHalfSize = {0.5f * std::abs(playerScale.x), 1.0f * std::abs(playerScale.y), 0.5f * std::abs(playerScale.z)};

	const AABB playerAABB = {
	    {playerPosition.x - playerHalfSize.x, playerPosition.y - playerHalfSize.y, playerPosition.z - playerHalfSize.z},
	    {playerPosition.x + playerHalfSize.x, playerPosition.y + playerHalfSize.y, playerPosition.z + playerHalfSize.z}
    };

	if (tutorialStageGate_->IsTouchingPlayer(playerAABB)) {
		selectedStage_ = StageNames::TUTORIAL_STAGE;
		SceneManager::GetInstance()->ChangeScene("Tutorial");
		return;
	}

	if (stage1Gate_->IsTouchingPlayer(playerAABB)) {
		selectedStage_ = StageNames::STAGE1;
		SceneManager::GetInstance()->ChangeScene("Game");
		return;
	}
}

void StageSelectScene::Draw() {
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::Skybox);

	skyBox_->Draw();

	Object3dCommon::GetInstance()->DrawCommon();

	player_->Draw(false);
	tutorialStageGate_->Draw();
	stage1Gate_->Draw();
	field_->Draw();

	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningOutline);

	player_->Draw(true);

	Object3dCommon::GetInstance()->EndOutlineDraw();
}
#include "GameScene.h"
#include "CameraController/CameraController.h"
#include "GameTimer/GameTimer.h"
#include "Model/ModelManager.h"
#include "Object/Background/SkyDome.h"
#include "Object/Boss/Boss.h"
#include "Object/Enemy/EnemyManager.h"
#include "Object/ExpCube/ExpCubeManager.h"
#include "Object/Player/Player.h"
#include "Object3d/Object3dCommon.h"
#include "OpenWorld/OpenWorld.h"
#include "ParticleManager.h"
#include "Rasen/Rasen.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include <algorithm>
#include <cmath>
#include <numbers>

GameScene::GameScene() {
	characterModel.LoadModel();
	cameraController = std::make_unique<CameraController>();
	particles = std::make_unique<Particles>();
	skyDome = std::make_unique<SkyDome>();
	player = std::make_unique<Player>();
	boss_ = std::make_unique<Boss>();
	rasen_ = std::make_unique<Rasen>();
	openWorld_ = std::make_unique<OpenWorld>();

	field = std::make_unique<MapchipField>();
	sceneTransition = std::make_unique<SceneTransition>();
	uimanager = std::make_unique<UIManager>();
	/*BG = std::make_unique<Background>();*/

	pause = std::make_unique<Pause>();
	BGMData = Audio::GetInstance()->SoundLoadFile("Resources/audio/BGM/Tailshaft.mp3");
	Audio::GetInstance()->SetSoundVolume(&BGMData, 0.3f);
	GameTimer::GetInstance()->Reset();
	Input::GetInstance()->SetIsCursorStability(true);
	Input::GetInstance()->SetIsCursorVisible(false);
	characterDisplay_ = std::make_unique<CharacterDisplay>();
}

GameScene::~GameScene() {}

void GameScene::Finalize() {

	Audio::GetInstance()->SoundUnload(&BGMData);

	rasen_->Finalize();
	openWorld_->Finalize();
}

void GameScene::Initialize() {

	sceneEndClear = false;
	sceneEndOver = false;
	isBGMPlaying = false;
	isCharacterDisplayMode_ = false;
	cameraController->Initialize();

	Object3dCommon::GetInstance()->SetDefaultCamera(cameraController->GetCamera());

	skyDome->Initialize(cameraController->GetCamera());
	player->Initialize(cameraController->GetCamera());

	field->LoadFromCSV("Resources/CSV/MapChip_stage1.csv");
	field->Initialize(cameraController->GetCamera());
	sceneTransition->Initialize(false);
	isTransitionIn = true;
	isTransitionOut = false;
	nextSceneName.clear();
	uimanager->SetPlayerHPMax(player->GetHPMax());
	uimanager->SetPlayerHP(player->GetHP());
	uimanager->Initialize();
	rasen_->Initialize(cameraController->GetCamera());
	openWorld_->Initialize(cameraController->GetCamera());
	playAreaMode_ = PlayAreaMode::kOpenWorld;

	activePointLightCount_ = 3;
	pointLights_[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
	pointLights_[0].position = {-75.0f, 10.0f, -75.0f};
	pointLights_[0].intensity = 1.0f;
	pointLights_[0].radius = 10.0f;
	pointLights_[0].decay = 0.7f;
	pointLights_[1].color = {1.0f, 0.9f, 0.9f, 1.0f};
	pointLights_[1].position = {75.0f, 5.0f, 75.0f};
	pointLights_[1].intensity = 0.0f;
	pointLights_[1].radius = 10.0f;
	pointLights_[1].decay = 0.7f;
	pointLights_[2].color = {0.4f, 0.4f, 1.0f, 1.0f};
	pointLights_[2].position = {-75.0f, 5.0f, 75.0f};
	pointLights_[2].intensity = 1.0f;
	pointLights_[2].radius = 5.0f;
	pointLights_[2].decay = 0.7f;


	directionalLight_.color = {76.0f/255.0f, 96.0f/255.0f, 178/255.0f, 1.0f};
	directionalLight_.direction = {0.0f, -1.0f, 0.5f};
	directionalLight_.intensity = 1.0f;

	activeSpotLightCount_ = 1;
	spotLights_[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
	spotLights_[0].position = {-50.0f, 5.0f, -50.0f};
	spotLights_[0].direction = {0.0f, 1.0f, 0.0f};
	spotLights_[0].intensity = 0.0f;
	spotLights_[0].distance = 7.0f;
	spotLights_[0].decay = 2.0f;
	spotLights_[0].cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLights_[0].cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
	pause->Initialize();
	pause->SetCurrentCharacterObj(player->GetCharacterObject3d());
	characterDisplay_->Initialize();
	characterDisplay_->SetActive(false);
}

void GameScene::DebugImGui() {

#ifdef USE_IMGUI
	if (ImGui::Begin("SampleLight")) {
		if (ImGui::TreeNode("DirectionalLight")) {
			ImGui::ColorEdit4("LightColor", &directionalLight_.color.x);
			ImGui::DragFloat3("LightDirection", &directionalLight_.direction.x, 0.1f, -1.0f, 1.0f);
			ImGui::DragFloat("LightIntensity", &directionalLight_.intensity, 0.1f, 0.0f, 10.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("PointLight")) {

			for (uint32_t index = 0; index < activePointLightCount_; ++index) {
				ImGui::PushID(static_cast<int>(index));
				if (ImGui::TreeNode("PointLight")) {
					ImGui::ColorEdit4("PointLightColor", &pointLights_[index].color.x);
					ImGui::DragFloat("PointLightIntensity", &pointLights_[index].intensity, 0.1f);
					ImGui::DragFloat3("PointLightPosition", &pointLights_[index].position.x, 0.1f);
					ImGui::DragFloat("PointLightRadius", &pointLights_[index].radius, 0.1f);
					ImGui::DragFloat("PointLightDecay", &pointLights_[index].decay, 0.1f);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("SpotLight")) {
			ImGui::ColorEdit4("SpotLightColor", &spotLights_[0].color.x);
			ImGui::DragFloat("SpotLightIntensity", &spotLights_[0].intensity, 0.1f);
			ImGui::DragFloat3("SpotLightPosition", &spotLights_[0].position.x, 0.1f);
			ImGui::DragFloat3("SpotLightDirection", &spotLights_[0].direction.x, 0.1f);
			ImGui::DragFloat("SpotLightDistance", &spotLights_[0].distance, 0.1f);
			ImGui::DragFloat("SpotLightDecay", &spotLights_[0].decay, 0.1f);
			ImGui::DragFloat("SpotLightCosAngle", &spotLights_[0].cosAngle, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("SpotLightCosFalloffStart", &spotLights_[0].cosFalloffStart, 0.1f, 0.0f, 1.0f);
			ImGui::TreePop();
		}
	}
	ImGui::Text("Play Area: %s", playAreaMode_ == PlayAreaMode::kSpiral ? "Spiral" : "OpenWorld");
	ImGui::Text("Press TAB to switch area mode");
	ImGui::End();
	rasen_->DebugImGui(boss_.get(), cameraController->GetCamera());

#endif // USE_IMGUI
}

void GameScene::Update() {
	GameTimer::GetInstance()->Update();
	const bool isAltPressed = Input::GetInstance()->PushKey(DIK_LMENU) || Input::GetInstance()->PushKey(DIK_RMENU);
	if (isAltPressed) {
		Input::GetInstance()->SetIsCursorStability(false);
		Input::GetInstance()->SetIsCursorVisible(true);
	} else {
		Input::GetInstance()->SetIsCursorStability(true);
		Input::GetInstance()->SetIsCursorVisible(false);
	}
	if (!isBGMPlaying) {
		Audio::GetInstance()->SoundPlayWave(BGMData, true);
		isBGMPlaying = true;
	}
	if (!isTransitionIn && !isTransitionOut && Input::GetInstance()->TriggerKey(DIK_TAB)) {
		playAreaMode_ = (playAreaMode_ == PlayAreaMode::kSpiral) ? PlayAreaMode::kOpenWorld : PlayAreaMode::kSpiral;
		isPause = false;
	}

	if ((playAreaMode_ != PlayAreaMode::kSpiral || !rasen_->IsLevelSelecting()) && !isTransitionIn && !isTransitionOut) {
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isCharacterDisplayMode_ = !isCharacterDisplayMode_;
			if (isCharacterDisplayMode_) {
				isPause = false;
			}
			characterDisplay_->SetActive(isCharacterDisplayMode_);
			if (isCharacterDisplayMode_) {
				Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight_);
			}
		}

		if (!isCharacterDisplayMode_) {
			bool togglePause = Input::GetInstance()->TriggerKey(DIK_ESCAPE) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonStart);
			if (togglePause) {
				isPause = !isPause;
			}
		}
	}
	DebugImGui();
	pause->Update(isPause);
	Pause::Action pauseAction = pause->ConsumeAction();
	if (pauseAction == Pause::Action::kResume) {
		isPause = false;
	} else if (pauseAction == Pause::Action::kTitle) {
		if (!isTransitionOut) {
			nextSceneName = "Title";
			sceneTransition->Initialize(true);
			isTransitionOut = true;
			isPause = false;
		}
		return;
	}

	if (isPause && !isTransitionOut) {
		return;
	}
	if (isCharacterDisplayMode_ && !isTransitionOut) {
		characterDisplay_->Update();
		return;
	}
	if (player->GetIsSkillAttack()) {
		pointLights_[1].intensity = 1.0f;
		pointLights_[1].position = {player->GetSkillPosition().x, player->GetSkillPosition().y + 4, player->GetSkillPosition().z};

	} else {
		pointLights_[1].intensity = 0.0f;
		pointLights_[1].position = {player->GetPosition().x, player->GetPosition().y + 4, player->GetPosition().z};
	}
	pointLights_[2].position = {player->GetPosition().x, player->GetPosition().y + 2, player->GetPosition().z};

	Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight_);
	Object3dCommon::GetInstance()->SetPointLights(pointLights_.data(), activePointLightCount_);
	Object3dCommon::GetInstance()->SetSpotLights(spotLights_.data(), activeSpotLightCount_);

	skyDome->SetCamera(cameraController->GetCamera());
	player->SetCamera(cameraController->GetCamera());
	field->SetCamera(cameraController->GetCamera());

	ParticleManager::GetInstance()->Update(cameraController->GetCamera());
	skyDome->Update();
	field->Update();
	player->Update();
	if (playAreaMode_ == PlayAreaMode::kSpiral) {
		rasen_->Update(cameraController->GetCamera(), player.get(), boss_.get());
		if (rasen_->IsLevelSelecting()) {
			return;
		}

		if (rasen_->IsGoalActive() && player->GetIsAlive() && !isTransitionOut) {
			nextSceneName = "Result";
			sceneTransition->Initialize(true);
			isTransitionOut = true;
		}
	} else {
		openWorld_->Update(cameraController->GetCamera(), player.get());
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_P)) {
		nextSceneName = "Result";
		sceneTransition->Initialize(true);
		isTransitionOut = true;
	}
#endif // _DEBUG

	if (playAreaMode_ == PlayAreaMode::kSpiral) {
		if (!player->GetIsAlive() || rasen_->GetHouse()->GetHP() == 0) {
			if (!isTransitionOut) {
				nextSceneName = "GameOver";
				sceneTransition->Initialize(true);
				isTransitionOut = true;
			}
		}

		// ===== プレイヤーと敵の当たり判定 =====
		Boss* activeBoss = (rasen_->IsBossActive() && boss_->GetIsAlive()) ? boss_.get() : nullptr;
		collisionManager_.HandleGameSceneCollisions(*player, *rasen_->GetEnemyManager(), *rasen_->GetExpCubeManager(), *rasen_->GetHouse(), activeBoss);
		if (player->ConsumeDamageTrigger()) {
			cameraController->StartShake(0.75f);
		}
	}
	uimanager->SetPlayerParameters(player->GetParameters());
	uimanager->SetPlayerHP(player->GetHP());

	uimanager->Update();

	particles->SetCameraPos(cameraController->GetCamera()->GetTranslate());
	particles->SetPlayerPos(player->GetPosition());
	particles->Update();

	cameraController->SetPlayerPos(player->GetPosition());

	cameraController->Update();

	if (isTransitionIn || isTransitionOut) {
		sceneTransition->Update();
		if (sceneTransition->IsEnd() && isTransitionIn) {
			isTransitionIn = false;
		}
		if (sceneTransition->IsEnd() && isTransitionOut) {
			SceneManager::GetInstance()->ChangeScene(nextSceneName);
		}
	}
}

void GameScene::Draw() {
	if (isCharacterDisplayMode_) {
		characterDisplay_->Draw();
		return;
	}
	if (isPause) {
		pause->Draw();
		if (isTransitionIn || isTransitionOut) {
			sceneTransition->Draw();
		}
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon();
	skyDome->Draw();
	Object3dCommon::GetInstance()->DrawCommonMirror();
	field->Draw();

	player->Draw();

	if (playAreaMode_ == PlayAreaMode::kSpiral) {
		rasen_->Draw(boss_.get());
		if (rasen_->IsBossActive()) {
			Object3dCommon::GetInstance()->DrawCommon();
		}
	} else {
		openWorld_->Draw();
	}
	particles->Draw();

	SpriteCommon::GetInstance()->DrawCommon();
	uimanager->Draw();

	pause->Draw();
	if (isTransitionIn || isTransitionOut) {
		sceneTransition->Draw();
	}
}
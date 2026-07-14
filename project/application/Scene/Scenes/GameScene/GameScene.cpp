#define NOMINMAX
#include "GameScene.h"
#include "AudioManager/BGMManager/BGMManager.h"
#include "CameraController/CameraController.h"
#include "Engine/Editor/EditorManager/EditorManager.h"
#include "GameTimer/GameTimer.h"
#include "Object/Background/Sky.h"
#include "Object/Boss/Boss.h"
#include "Object/Characters/Enemy/EnemyManager.h"
#include "Object/Player/Player.h"
#include "Object3d/Object3dCommon.h"
#include "OpenWorld/OpenWorld.h"
#include "ParticleManager.h"
#include "PlayCommand/PlayCommand.h"
#include "Rasen/Rasen.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace {
constexpr float kLockOnTargetMaxAngle = std::numbers::pi_v<float> / 4.0f;
constexpr int kDefaultFullscreenFilterType = 1;
constexpr int kRadialBlurFullscreenFilterType = 2;
constexpr Vector2 kDashRadialBlurCenter = {0.5f, 0.5f};
constexpr float kDashRadialBlurWidth = 0.04f;
constexpr int kDashRadialBlurSampleCount = 2;

Enemy* TryFindNearestAliveEnemy(Player& player, EnemyManager& enemyManager) {
	const Vector3 playerPos = player.GetPosition();
	Enemy* nearestEnemy = nullptr;
	float nearestDistanceSq = 0.0f;

	for (const auto& enemy : enemyManager.GetEnemies()) {
		if (!enemy || !enemy->GetIsAlive() || enemy->IsDying() || enemy->GetHP() <= 0) {
			continue;
		}

		Vector3 toEnemy = enemy->GetPosition() - playerPos;
		toEnemy.y = 0.0f;
		const float distanceSq = Function::LengthSquared(toEnemy);
		if (!nearestEnemy || distanceSq < nearestDistanceSq) {
			nearestEnemy = enemy.get();
			nearestDistanceSq = distanceSq;
		}
	}

	return nearestEnemy;
}

Enemy* TryFindNearestEnemyInPlayerFront(Player& player, EnemyManager& enemyManager) {
	const Vector3 playerPos = player.GetPosition();
	const float playerYaw = player.GetRotate().y;
	const Vector3 playerForward = {std::sinf(playerYaw), 0.0f, std::cosf(playerYaw)};
	const float minForwardDot = std::cos(kLockOnTargetMaxAngle);

	Enemy* nearestEnemy = nullptr;
	float nearestDistanceSq = 0.0f;

	for (const auto& enemy : enemyManager.GetEnemies()) {
		if (!enemy || !enemy->GetIsAlive() || enemy->IsDying() || enemy->GetHP() <= 0) {
			continue;
		}

		Vector3 toEnemy = enemy->GetPosition() - playerPos;
		toEnemy.y = 0.0f;
		const float distanceSq = Function::LengthSquared(toEnemy);
		if (distanceSq < 0.0001f) {
			continue;
		}

		const Vector3 directionToEnemy = Function::Normalize(toEnemy);
		const float forwardDot = Function::Dot(playerForward, directionToEnemy);
		if (forwardDot < minForwardDot) {
			continue;
		}

		if (!nearestEnemy || distanceSq < nearestDistanceSq) {
			nearestEnemy = enemy.get();
			nearestDistanceSq = distanceSq;
		}
	}

	return nearestEnemy;
}

void ApplyLockOnMarker(EnemyManager& enemyManager, Enemy* lockOnEnemy) {
	for (const auto& enemy : enemyManager.GetEnemies()) {
		if (enemy) {
			enemy->SetLockOn(enemy.get() == lockOnEnemy && enemy->GetIsAlive() && !enemy->IsDying() && enemy->GetHP() > 0);
		}
	}
}

bool ContainsLockOnEnemy(EnemyManager& enemyManager, Enemy* lockOnEnemy) {
	if (!lockOnEnemy) {
		return false;
	}
	for (const auto& enemy : enemyManager.GetEnemies()) {
		if (enemy.get() == lockOnEnemy) {
			return enemy->GetIsAlive() && !enemy->IsDying() && enemy->GetHP() > 0;
		}
	}
	return false;
}
} // namespace

GameScene::GameScene() {
	Object3dCommon::GetInstance()->SetEnvironmentMapTexture("Resources/SkyBox/sky.dds");
	characterModel.LoadModel();
	cameraController = std::make_unique<CameraController>();
	skyDome = std::make_unique<Sky>();
	player = std::make_unique<Player>();
	boss_ = std::make_unique<Boss>();
	rasen_ = std::make_unique<Rasen>();
	openWorld_ = std::make_unique<OpenWorld>();

	field = std::make_unique<Field>();
	sceneTransition = std::make_unique<SceneTransition>();
	uimanager = std::make_unique<UIManager>();

	pause = std::make_unique<Pause>();
	GameTimer::GetInstance()->Reset();
	Input::GetInstance()->SetIsCursorStability(true);
	Input::GetInstance()->SetIsCursorVisible(false);
	characterDisplay_ = std::make_unique<CharacterDisplay>();
	team_ = std::make_unique<Team>();
}

GameScene::~GameScene() {}

void GameScene::Finalize() {
	UnloadTeamDisplay();
	rasen_->Finalize();
	openWorld_->Finalize();
	ParticleManager::GetInstance()->Clear();
}

void GameScene::Initialize() {

	sceneEndClear = false;
	sceneEndOver = false;
	isCharacterDisplayMode_ = false;
	isPartyMode_ = false;
	cameraController->Initialize();

	Object3dCommon::GetInstance()->SetDefaultCamera(cameraController->GetCamera());

	skyDome->Initialize(cameraController->GetCamera());
	player->Initialize(cameraController->GetCamera());

	field->Initialize(cameraController->GetCamera());
	sceneTransition->Initialize(false);
	isTransitionIn = true;
	isTransitionOut = false;
	nextSceneName.clear();
	team_->Initialize();
	uimanager->SetTeam(team_.get());
	uimanager->Initialize();
	uimanager->SetPlayerHPMax(team_->GetActiveCharacterHPMax());
	uimanager->SetPlayerHP(team_->GetActiveCharacterHP());
	uimanager->SetPlayerDashGauge(player->GetDashGauge(), player->GetDashGaugeMax());
	rasen_->Initialize(cameraController->GetCamera());
	openWorld_->Initialize(cameraController->GetCamera());
	playAreaMode_ = PlayAreaMode::kSpiral;

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
	pause->SetCurrentAttribute(player->GetCurrentAttribute());
	characterDisplay_->Initialize();
	characterDisplay_->SetActive(false);
	UnloadTeamDisplay();
	
	vinettColor_ = {255, 255, 255};
	vinettStrength_ = 10.0f;
	Object3dCommon::GetInstance()->SetVignetteStrength(vinettStrength_);
	hitVinettTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Effect/HitVinett.png");
	hitVinettSprite_ = std::make_unique<Sprite>();
	hitVinettSprite_->Initialize(hitVinettTextureHandle_);
	hitVinettSprite_->SetPosition({0.0f, 0.0f});
	hitVinettSprite_->SetScale({static_cast<float>(WinApp::kClientWidth), static_cast<float>(WinApp::kClientHeight)});
	hitVinettSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
	hitVinettSprite_->Update();
	hitVinettTimer_ = 0.0f;
	introBlurStartKernelSize_ = 15.0f;
	introBlurDelayTimer_ = 0.0f;
	fullscreenFilterType_ = kDefaultFullscreenFilterType;
	radialBlurCenter_ = kDashRadialBlurCenter;
	radialBlurWidth_ = kDashRadialBlurWidth;
	radialBlurSampleCount_ = kDashRadialBlurSampleCount;
	Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
	Object3dCommon::GetInstance()->SetRadialBlurCenter(radialBlurCenter_);
	Object3dCommon::GetInstance()->SetRadialBlurWidth(radialBlurWidth_);
	Object3dCommon::GetInstance()->SetRadialBlurSampleCount(radialBlurSampleCount_);
	dissolveEnabled_ = false;
	dissolveThreshold_ = 0.0f;
	Object3dCommon::GetInstance()->SetDissolveEnabled(dissolveEnabled_);
	Object3dCommon::GetInstance()->SetDissolveThreshold(dissolveThreshold_);
	Object3dCommon::GetInstance()->SetDissolveEdgeWidth(dissolveEdgeWidth_);
	lockOnMarkerEnemy_ = nullptr;
	lockOnMarkerTimer_ = 0.0f;
}

void GameScene::DebugImGui() {

#ifdef USE_IMGUI
	if (ImGui::Begin("vinett")) {
		ImGui::ColorEdit3("vinettcolor", &vinettColor_.x);
		ImGui::DragFloat("vinnettstrength", &vinettStrength_, 0.1f);
		const char* filterTypes[] = {"Box", "Gaussian", "Radial"};
		if (ImGui::Combo("Fullscreen Filter", &fullscreenFilterType_, filterTypes, IM_ARRAYSIZE(filterTypes))) {
			Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
		}
		if (ImGui::DragFloat("Gaussian Sigma", &gaussianFilterSigma_, 0.01f, 0.001f, 10.0f)) {
			Object3dCommon::GetInstance()->SetGaussianFilterSigma(gaussianFilterSigma_);
		}
		if (ImGui::DragFloat2("Radial Blur Center", &radialBlurCenter_.x, 0.01f, 0.0f, 1.0f)) {
			Object3dCommon::GetInstance()->SetRadialBlurCenter(radialBlurCenter_);
		}
		if (ImGui::DragFloat("Radial Blur Width", &radialBlurWidth_, 0.001f, 0.0f, 0.2f)) {
			Object3dCommon::GetInstance()->SetRadialBlurWidth(radialBlurWidth_);
		}
		if (ImGui::DragInt("Radial Blur Samples", &radialBlurSampleCount_, 1, 1, 32)) {
			Object3dCommon::GetInstance()->SetRadialBlurSampleCount(radialBlurSampleCount_);
		}
		if (ImGui::Checkbox("Dissolve Enabled", &dissolveEnabled_)) {
			Object3dCommon::GetInstance()->SetDissolveEnabled(dissolveEnabled_);
		}
		if (ImGui::SliderFloat("Dissolve Threshold", &dissolveThreshold_, 0.0f, 1.0f)) {
			Object3dCommon::GetInstance()->SetDissolveThreshold(dissolveThreshold_);
		}
		if (ImGui::SliderFloat("Dissolve Edge Width", &dissolveEdgeWidth_, 0.0f, 0.5f)) {
			Object3dCommon::GetInstance()->SetDissolveEdgeWidth(dissolveEdgeWidth_);
		}
		Object3dCommon::GetInstance()->SetVignetteColor(vinettColor_);
		Object3dCommon::GetInstance()->SetVignetteStrength(vinettStrength_);
	}
	ImGui::End();
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
void GameScene::LoadTeamDisplay() {
	if (teamDisplay_) {
		return;
	}
	teamDisplay_ = std::make_unique<TeamDisplay>();
	teamDisplay_->Initialize(*team_);
}

void GameScene::UnloadTeamDisplay() {
	if (!teamDisplay_) {
		return;
	}
	teamDisplay_->Unload();
	teamDisplay_.reset();
}
void GameScene::Update() {
	GameTimer::GetInstance()->Update();
	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();
	vinettStrength_ -= 0.3f;
	vinettStrength_ = std::max(0.0f, vinettStrength_);
	Object3dCommon::GetInstance()->SetVignetteStrength(vinettStrength_);
	if (introBlurDelayTimer_ < kIntroBlurDelaySeconds_) {
		introBlurDelayTimer_ += deltaTime;
	} else {
		const float fadeElapsed = introBlurDelayTimer_ - kIntroBlurDelaySeconds_;
		const float t = std::clamp(fadeElapsed / kIntroBlurFadeDurationSeconds_, 0.0f, 1.0f);
		const float kernelSizeFloat = std::lerp(introBlurStartKernelSize_, 1.0f, t);
		Object3dCommon::GetInstance()->SetBoxFilterKernelSize(static_cast<int>(std::round(kernelSizeFloat)));
		introBlurDelayTimer_ += deltaTime;
	}
	const bool isAltPressed = Input::GetInstance()->PushKey(DIK_LMENU) || Input::GetInstance()->PushKey(DIK_RMENU);
	if (isAltPressed) {
		Input::GetInstance()->SetIsCursorStability(false);
		Input::GetInstance()->SetIsCursorVisible(true);
	} else {
		Input::GetInstance()->SetIsCursorStability(true);
		Input::GetInstance()->SetIsCursorVisible(false);
	}
	
	BGMManager::GetInstance()->Play(BGMManager::BGMType::Game);
	if (!isTransitionIn && !isTransitionOut && Input::GetInstance()->TriggerKey(DIK_TAB)) {
		playAreaMode_ = (playAreaMode_ == PlayAreaMode::kSpiral) ? PlayAreaMode::kOpenWorld : PlayAreaMode::kSpiral;
		isPause = false;
	}

	if ((playAreaMode_ != PlayAreaMode::kSpiral || !rasen_->IsLevelSelecting()) && !isTransitionIn && !isTransitionOut) {
		if (PlayCommand::GetCharacterDisplay()) {
			isCharacterDisplayMode_ = !isCharacterDisplayMode_;
			if (isCharacterDisplayMode_) {
				isPause = false;
				if (isPartyMode_) {
					isPartyMode_ = false;
					UnloadTeamDisplay();
				}
			}
			characterDisplay_->SetActive(isCharacterDisplayMode_);
			if (isCharacterDisplayMode_) {
				Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight_);
			}
		}

		if (!isCharacterDisplayMode_) {
			if (PlayCommand::GetTeamSelectDisplay()&&!isPause) {
				isPartyMode_ = !isPartyMode_;
				if (isPartyMode_) {
					LoadTeamDisplay();
					Input::GetInstance()->SetIsCursorStability(false);
					Input::GetInstance()->SetIsCursorVisible(true);
				} else {
					UnloadTeamDisplay();
				}
			}
			if (!isPartyMode_) {
				bool togglePause = Input::GetInstance()->TriggerKey(DIK_ESCAPE) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonStart);
				if (togglePause) {
					isPause = !isPause;
				}
			}
			if (isPartyMode_ && !PlayCommand::GetTeamSelectDisplay()) {
				if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)||Input::GetInstance()->TriggerButton(Input::PadButton::kButtonB)) {
					isPartyMode_ = false;
					UnloadTeamDisplay();
				}
			}
		}
	}
	DebugImGui();
	if (!isCharacterDeathDissolving_) {
		team_->Update();
	}
	if (isPartyMode_ && teamDisplay_) {
		teamDisplay_->Update(*team_);
	}
	if (team_->ConsumeCharacterSwitchTriggered()) {
		player->SetCharacterType(team_->GetActiveCharacterName());
		pause->SetCurrentCharacterObj(player->GetCharacterObject3d());
		pause->SetCurrentAttribute(player->GetCurrentAttribute());
	}
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
		fullscreenFilterType_ = kDefaultFullscreenFilterType;
		Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
		return;
	}
	if (isPartyMode_ && !isTransitionOut) {
		fullscreenFilterType_ = kDefaultFullscreenFilterType;
		Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
		return;
	}
	if (isCharacterDisplayMode_ && !isTransitionOut) {
		fullscreenFilterType_ = kDefaultFullscreenFilterType;
		Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
		characterDisplay_->Update();
		return;
	}
	if (isCharacterDeathDissolving_) {
		characterDeathDissolveTimer_ += deltaTime;
		dissolveEnabled_ = true;
		dissolveThreshold_ = std::clamp(characterDeathDissolveTimer_ / kCharacterDeathDissolveDuration_, 0.0f, 1.0f);
		const float dissolveAlpha = std::clamp(1.0f - (characterDeathDissolveTimer_ / kCharacterDeathDissolveDuration_), 0.0f, 1.0f);
		if (Object3d* characterObject = player->GetCharacterObject3d()) {
			Vector4 dissolveColor = characterDeathDissolveStartColor_;
			dissolveColor.w *= dissolveAlpha;
			characterObject->SetColor(dissolveColor);
			characterObject->SetDissolveEnabled(dissolveEnabled_);
			characterObject->SetDissolveThreshold(dissolveThreshold_);
			characterObject->SetDissolveEdgeWidth(dissolveEdgeWidth_);
			characterObject->SetDissolveEdgeColor(dissolveEdgeColor_);
		}

		if (characterDeathDissolveTimer_ >= kCharacterDeathDissolveDuration_) {
			isCharacterDeathDissolving_ = false;
			characterDeathDissolveTimer_ = 0.0f;
			dissolveEnabled_ = false;
			dissolveThreshold_ = 0.0f;
			if (Object3d* characterObject = player->GetCharacterObject3d()) {
				characterObject->SetColor(characterDeathDissolveStartColor_);
				characterObject->SetDissolveEnabled(false);
				characterObject->SetDissolveThreshold(0.0f);
			}
			if (team_->SwitchToNextAliveMember()) {
				player->SetCharacterType(team_->GetActiveCharacterName());
				if (Object3d* characterObject = player->GetCharacterObject3d()) {
					characterObject->SetColor(characterDeathDissolveStartColor_);
					characterObject->SetDissolveEnabled(false);
					characterObject->SetDissolveThreshold(0.0f);
				}
				pause->SetCurrentCharacterObj(player->GetCharacterObject3d());
				pause->SetCurrentAttribute(player->GetCurrentAttribute());
			}
		}
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
	if (playAreaMode_ == PlayAreaMode::kSpiral && PlayCommand::GetNORMAL_ATTACK_TRIGGER()) {
		if (Enemy* nearestEnemy = TryFindNearestAliveEnemy(*player, *rasen_->GetEnemyManager())) {
			player->SetAttackApproachTarget(nearestEnemy->GetPosition());
		}
	}
	player->Update();
	const bool isDashing = player->IsDashing();
	fullscreenFilterType_ = isDashing ? kRadialBlurFullscreenFilterType : kDefaultFullscreenFilterType;
	Object3dCommon::GetInstance()->SetFullscreenFilterType(fullscreenFilterType_);
	if (playAreaMode_ == PlayAreaMode::kSpiral) {
		rasen_->Update(cameraController->GetCamera(), player.get(), boss_.get());
		EnemyManager* enemyManager = rasen_->GetEnemyManager();
		if (enemyManager != nullptr && enemyManager->GetAliveEnemyCount() == 0 && enemyManager->IsWaveComplete()) {
			cameraController->ClearAttackCameraTarget();
			ApplyLockOnMarker(*enemyManager, nullptr);
			lockOnMarkerEnemy_ = nullptr;
			lockOnMarkerTimer_ = 0.0f;
		}
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
		if (team_->GetAreAllMembersDead() || rasen_->GetHouse()->GetHP() == 0) {
			if (!isTransitionOut) {
				nextSceneName = "GameOver";
				sceneTransition->Initialize(true);
				isTransitionOut = true;
			}
		}

		// ===== プレイヤーと敵の当たり判定 =====
		Boss* activeBoss = (rasen_->IsBossActive() && boss_->GetIsAlive()) ? boss_.get() : nullptr;
		Vector3 hitEnemyPos{};
		bool didPlayerAttackHitEnemy = false;
		const bool didNormalAttackHitEnemy = collisionManager_.HandleGameSceneCollisions(*player, *rasen_->GetEnemyManager(), *rasen_->GetHouse(), activeBoss, &hitEnemyPos, &didPlayerAttackHitEnemy);
		if (didPlayerAttackHitEnemy) {
			hitVinettTimer_ = kHitVinettDuration_;
		}
		if (didNormalAttackHitEnemy) {
			Enemy* cameraTargetEnemy = TryFindNearestEnemyInPlayerFront(*player, *rasen_->GetEnemyManager());
			if (!cameraTargetEnemy) {
				cameraTargetEnemy = TryFindNearestAliveEnemy(*player, *rasen_->GetEnemyManager());
			}
			if (cameraTargetEnemy) {
				const Vector3 cameraTargetPos = cameraTargetEnemy->GetPosition();
				const int normalAttackComboStep = player->GetSword()->GetComboStep();
				if (normalAttackComboStep <= 1) {
					cameraController->SetLockOnTarget(cameraTargetPos, kLockOnMarkerDuration_);
					lockOnMarkerEnemy_ = cameraTargetEnemy;
					lockOnMarkerTimer_ = kLockOnMarkerDuration_;
				} else {
					cameraController->SetNormalAttackTarget(cameraTargetPos);
					lockOnMarkerEnemy_ = nullptr;
					lockOnMarkerTimer_ = 0.0f;
				}
			} else {
				cameraController->ClearAttackCameraTarget();
				lockOnMarkerEnemy_ = nullptr;
				lockOnMarkerTimer_ = 0.0f;
			}
		}
		if (player->ConsumeDamageTrigger()) {
			team_->DamageActiveCharacter(player->ConsumePendingDamage());
			cameraController->StartShake(0.75f);
			if (!team_->GetIsActiveCharacterAlive()) {
				damageGrayscaleTimer_ = 0.0f;
				Object3dCommon::GetInstance()->SetFullScreenGrayscaleEnabled(false);
			} else {
				damageGrayscaleTimer_ = kDamageGrayscaleDuration_;
			}
			if (!team_->GetIsActiveCharacterAlive() && !team_->GetAreAllMembersDead()) {
				isCharacterDeathDissolving_ = true;
				characterDeathDissolveTimer_ = 0.0f;
				dissolveEnabled_ = true;
				dissolveThreshold_ = 0.0f;
				if (Object3d* characterObject = player->GetCharacterObject3d()) {
					characterDeathDissolveStartColor_ = characterObject->GetColor();
					characterObject->SetDissolveEnabled(dissolveEnabled_);
					characterObject->SetDissolveThreshold(dissolveThreshold_);
					characterObject->SetDissolveEdgeWidth(dissolveEdgeWidth_);
					characterObject->SetDissolveEdgeColor(dissolveEdgeColor_);
				}
			}
		}
	}
	if (hitVinettTimer_ > 0.0f) {
		hitVinettTimer_ = std::max(0.0f, hitVinettTimer_ - deltaTime);
	}
	if (hitVinettSprite_) {
		const float alpha = (kHitVinettDuration_ > 0.0f) ? (hitVinettTimer_ / kHitVinettDuration_) : 0.0f;
		hitVinettSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		hitVinettSprite_->Update();
	}
	if (damageGrayscaleTimer_ > 0.0f) {
		damageGrayscaleTimer_ -= GameBase::GetInstance()->GetDeltaTime();
		if (damageGrayscaleTimer_ < 0.0f) {
			damageGrayscaleTimer_ = 0.0f;
		}
	}
	Object3dCommon::GetInstance()->SetFullScreenGrayscaleEnabled(damageGrayscaleTimer_ > 0.0f);
	if (playAreaMode_ == PlayAreaMode::kSpiral && rasen_ && rasen_->GetEnemyManager()) {
		EnemyManager& enemyManager = *rasen_->GetEnemyManager();
		if (lockOnMarkerTimer_ > 0.0f) {
			lockOnMarkerTimer_ = std::max(0.0f, lockOnMarkerTimer_ - deltaTime);
		}
		if (lockOnMarkerTimer_ <= 0.0f || !ContainsLockOnEnemy(enemyManager, lockOnMarkerEnemy_)) {
			lockOnMarkerEnemy_ = nullptr;
			lockOnMarkerTimer_ = 0.0f;
		}
		ApplyLockOnMarker(enemyManager, lockOnMarkerEnemy_);
	}
	uimanager->SetPlayerParameters(player->GetParameters());
	uimanager->SetPlayerHPMax(team_->GetActiveCharacterHPMax());
	uimanager->SetPlayerHP(team_->GetActiveCharacterHP());
	uimanager->SetPlayerDashGauge(player->GetDashGauge(), player->GetDashGaugeMax());
	uimanager->Update();

	cameraController->SetPlayerPos(player->GetPosition());
	if (playAreaMode_ == PlayAreaMode::kSpiral && rasen_ && rasen_->GetHouse() && Input::GetInstance()->TriggerLeftTrigger()) {
		cameraController->LookAtFromPlayerPosition(rasen_->GetHouse()->GetPosition());
	}
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
	if (isPartyMode_) {
		SpriteCommon::GetInstance()->DrawCommon();
		if (teamDisplay_) {
			teamDisplay_->Draw(*team_);
		}
		if (isTransitionIn || isTransitionOut) {
			sceneTransition->Draw();
		}
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon();
	skyDome->Draw();
	Object3dCommon::GetInstance()->DrawCommon();
	field->Draw();

	player->Draw();
	EditorManager::GetInstance()->DrawEditorGridLines();
	if (playAreaMode_ == PlayAreaMode::kSpiral) {
		rasen_->Draw(boss_.get());
		if (rasen_->IsBossActive()) {
			Object3dCommon::GetInstance()->DrawCommon();
		}
	} else {
		openWorld_->Draw();
	}

	SpriteCommon::GetInstance()->DrawCommon();
	if (hitVinettTimer_ > 0.0f && hitVinettSprite_) {
		hitVinettSprite_->Draw();
	}
	uimanager->Draw();
	pause->Draw();
	if (isTransitionIn || isTransitionOut) {
		sceneTransition->Draw();
	}
}
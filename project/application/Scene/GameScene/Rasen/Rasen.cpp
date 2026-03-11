#include "Rasen.h"

#include "Input.h"
#include "Object/Boss/Boss.h"
#include "Object/Enemy/EnemyManager.h"
#include "Object/ExpCube/ExpCubeManager.h"
#include "Object/Player/Player.h"
#include "TextureManager.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

Rasen::Rasen() {
	house = std::make_unique<House>();
	enemyManager = std::make_unique<EnemyManager>();
	expCubeManager = std::make_unique<ExpCubeManager>();
}

void Rasen::Finalize() {
	for (int i = 0; i < 4; i++) {
		levelupIcons[i].reset();
	}
}

void Rasen::Initialize(Camera* camera) {
	enemyManager->Initialize(camera);
	expCubeManager->Initialize(camera);
	house->Initialize(camera);

	int handle[4]{};
	handle[0] = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/levelup_attack.png");
	handle[1] = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/levelup_speed.png");
	handle[2] = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/levelup_hp.png");
	handle[3] = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/levelup_allow.png");
	for (int i = 0; i < 4; i++) {
		levelupIcons[i] = std::make_unique<Sprite>();
		levelupIcons[i]->Initialize(handle[i]);
		levelupIcons[i]->SetScale({256, 256});
	}

	uint32_t phaseHandles[5] = {
	    TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/phase1.png"), TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/phase2.png"),
	    TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/phase3.png"), TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/phase4.png"),
	    TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/phase5.png"),
	};
	const uint32_t bossHpBarHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/BossHpBar.png");
	for (int i = 0; i < 5; i++) {
		phaseSprites_[i] = std::make_unique<Sprite>();
		phaseSprites_[i]->Initialize(phaseHandles[i]);
		phaseSprites_[i]->SetScale(phaseSpriteSize_);
	}
	bossHpBarSprite_ = std::make_unique<Sprite>();
	bossHpBarSprite_->Initialize(bossHpBarHandle);
	bossHpBarSprite_->SetAnchorPoint({0.0f, 0.0f});
	bossHpBarSprite_->SetColor({0.9f, 0.0f, 0.0f, 1.0f});
	bossHpBarSprite_->SetScale(bossHpBarSize_);
	bossHpBarSprite_->SetPosition({640.0f - bossHpBarSize_.x / 2.0f, phaseSpriteY_});
	bossHpBarBackSprite_ = std::make_unique<Sprite>();
	bossHpBarBackSprite_->Initialize(bossHpBarHandle);
	bossHpBarBackSprite_->SetAnchorPoint({0.0f, 0.0f});
	bossHpBarBackSprite_->SetScale(bossHpBarSize_);
	bossHpBarBackSprite_->SetColor({0.3f, 0.3f, 0.3f, 1.0f});
	bossHpBarBackSprite_->SetPosition({640.0f - bossHpBarSize_.x / 2.0f, phaseSpriteY_});

	debugPhaseSelection_ = std::clamp(enemyManager->GetCurrentWave(), 1, 5) - 1;
	lastWave_ = 0;
	isPhaseSpriteActive_ = false;
	isPhaseSpritePaused_ = false;
	isWarningActive_ = false;
	warningTimer_ = 0.0f;
	isBossActive_ = false;
	goalActive = false;
	isLevelSelecting = false;

	const uint32_t warningHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/warnig.png");
	warningSprite_ = std::make_unique<Sprite>();
	warningSprite_->Initialize(warningHandle);
	warningSprite_->SetScale(warningSpriteBaseScale_);
	warningSprite_->SetPosition({640.0f - warningSpriteBaseScale_.x / 2.0f, 360.0f - warningSpriteBaseScale_.y / 2.0f});
}

void Rasen::ApplyPhaseSelection(int selectionIndex, Camera* camera, Boss* boss) {
	isWarningActive_ = false;
	warningTimer_ = 0.0f;
	isPhaseSpriteActive_ = false;
	isPhaseSpritePaused_ = false;
	phaseSpriteStopTimer_ = 0.0f;
	phaseSpriteX_ = 0.0f;
	goalActive = false;

	const int bossIndex = 5;
	if (selectionIndex == bossIndex) {
		enemyManager->ForceBossPhase();
		isBossActive_ = true;
		boss->Initialize(camera, {10.0f, 2.5f, -40.0f});
		return;
	}

	isBossActive_ = false;
	const int waveNumber = selectionIndex + 1;
	enemyManager->ForceStartWave(waveNumber);
	lastWave_ = waveNumber - 1;
}

void Rasen::DebugImGui(Boss* boss, Camera* camera) {
#ifdef USE_IMGUI
	if (ImGui::Begin("Wave Info")) {
		ImGui::Text("Current Wave: %d", enemyManager->GetCurrentWave());
		ImGui::Text("Alive Enemies: %d", enemyManager->GetAliveEnemyCount());
		ImGui::Text("Total Killed: %d", enemyManager->GetTotalEnemiesKilled());
		ImGui::ProgressBar(enemyManager->GetWaveProgress(), ImVec2(0.0f, 0.0f));

		const char* stateNames[] = {"Waiting", "Spawning", "Active", "Complete"};
		int stateIndex = static_cast<int>(enemyManager->GetWaveState());
		ImGui::Text("Wave State: %s", stateNames[stateIndex]);

		float waveDelay = 3.0f;
		if (ImGui::DragFloat("Wave Delay", &waveDelay, 0.1f, 0.5f, 10.0f)) {
			enemyManager->SetWaveDelay(waveDelay);
		}
		ImGui::Separator();
		ImGui::Text("Phase Select");
		const char* phaseItems[] = {"Phase 1", "Phase 2", "Phase 3", "Phase 4", "Phase 5", "Boss"};
		ImGui::Combo("Phase", &debugPhaseSelection_, phaseItems, IM_ARRAYSIZE(phaseItems));
		if (ImGui::Button("Apply Phase")) {
			ApplyPhaseSelection(debugPhaseSelection_, camera, boss);
		}
		ImGui::End();
	}
#endif
}

void Rasen::Update(Camera* camera, Player* player, Boss* boss) {
	if (isLevelSelecting) {
		if (Input::GetInstance()->TriggerKey(DIK_A)) {
			cursorIndex = 0;
		}
		if (Input::GetInstance()->TriggerKey(DIK_D)) {
			cursorIndex = 1;
		}
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			int choice = selectChoices[cursorIndex];
			auto params = player->GetParameters();
			switch (choice) {
			case 0:
				params.AttuckUp++;
				break;
			case 1:
				params.SpeedUp++;
				break;
			case 2:
				params.HPUp++;
				break;
			case 3:
				params.AllowUp++;
				break;
			}
			player->SetParameters(params);
			player->IsLevelUp(false);
			isLevelSelecting = false;
		}
		return;
	}

	house->Update(camera);
	if (player->GetLv() && !isLevelSelecting) {
		isLevelSelecting = true;
		int a = rand() % 4;
		int b = rand() % 4;
		while (b == a) {
			b = rand() % 4;
		}
		selectChoices[0] = a;
		selectChoices[1] = b;
		cursorIndex = 0;
	}

	enemyManager->Update(camera, house->GetPosition(), house->GetScale(), player->GetPosition(), player->GetIsAlive());
	expCubeManager->Update(camera, player->GetMovementLimitCenter(), player->GetMovementLimitRadius());

	int currentWave = enemyManager->GetCurrentWave();
	if (currentWave != lastWave_) {
		lastWave_ = currentWave;
		currentPhaseSpriteIndex_ = std::clamp(currentWave, 1, 5) - 1;
		isPhaseSpriteActive_ = true;
		isPhaseSpritePaused_ = false;
		phaseSpriteStopTimer_ = 0.0f;
		phaseSpriteX_ = 1280.0f + phaseSpriteSize_.x;
	}

	if (isPhaseSpriteActive_ && !isBossActive_) {
		const float deltaTime = 1.0f / 60.0f;
		const float moveSpeed = 500.0f;
		const float stopDuration = 0.8f;
		const float centerX = 640.0f - phaseSpriteSize_.x / 2.0f;
		if (isPhaseSpritePaused_) {
			phaseSpriteStopTimer_ += deltaTime;
			if (phaseSpriteStopTimer_ >= stopDuration) {
				isPhaseSpritePaused_ = false;
			}
		} else {
			phaseSpriteX_ -= moveSpeed * deltaTime;
			if (phaseSpriteX_ <= centerX) {
				phaseSpriteX_ = centerX;
				isPhaseSpritePaused_ = true;
				phaseSpriteStopTimer_ = 0.0f;
			}
		}
		if (!isPhaseSpritePaused_ && phaseSpriteX_ <= -phaseSpriteSize_.x) {
			isPhaseSpriteActive_ = false;
		}
		auto* phaseSprite = phaseSprites_[currentPhaseSpriteIndex_].get();
		phaseSprite->SetPosition({phaseSpriteX_, phaseSpriteY_});
		phaseSprite->Update();
	}

	if (isBossActive_ && boss->GetIsAlive()) {
		const int bossMaxHp = boss->GetMaxHP();
		if (bossMaxHp > 0) {
			const float hpRatio = std::clamp(static_cast<float>(boss->GetHP()) / static_cast<float>(bossMaxHp), 0.0f, 1.0f);
			Vector2 bossHpBarScale = bossHpBarSize_;
			bossHpBarScale.x *= hpRatio;
			bossHpBarSprite_->SetScale(bossHpBarScale);
			bossHpBarSprite_->SetPosition({640.0f - bossHpBarSize_.x / 2.0f, phaseSpriteY_});
		}
		bossHpBarSprite_->Update();
		bossHpBarBackSprite_->Update();
	}

	if (enemyManager->AreAllWavesComplete() && !isBossActive_) {
		const float deltaTime = 1.0f / 60.0f;
		if (!isWarningActive_) {
			isWarningActive_ = true;
			warningTimer_ = 0.0f;
		}
		warningTimer_ += deltaTime;
		float pulse = 1.0f + std::sin(warningTimer_ * 6.0f) * 0.05f;
		warningSprite_->SetScale({warningSpriteBaseScale_.x * pulse, warningSpriteBaseScale_.y * pulse});
		warningSprite_->SetPosition({640.0f - warningSpriteBaseScale_.x * pulse / 2.0f, 360.0f - warningSpriteBaseScale_.y * pulse / 2.0f});
		warningSprite_->Update();
		if (warningTimer_ >= warningDuration_) {
			isWarningActive_ = false;
			isBossActive_ = true;
			boss->Initialize(camera, {10.0f, 2.5f, -40.0f});
		}
	}

	if (isBossActive_ && boss->GetIsAlive()) {
		boss->SetCamera(camera);
		boss->Update(house->GetPosition(), player->GetPosition(), player->GetIsAlive());
	}
	if (isBossActive_ && !boss->GetIsAlive()) {
		goalActive = true;
	}
}

void Rasen::Draw(Boss* boss) {
	enemyManager->Draw();
	if (isBossActive_) {
		boss->Draw();
	}
	expCubeManager->Draw();
	house->Draw();

	if (isPhaseSpriteActive_ && !isBossActive_) {
		phaseSprites_[currentPhaseSpriteIndex_]->Draw();
	}
	if (isBossActive_ && boss->GetIsAlive()) {
		bossHpBarBackSprite_->Draw();
		bossHpBarSprite_->Draw();
	}
	if (isWarningActive_) {
		warningSprite_->Draw();
	}
	if (isLevelSelecting) {
		int leftID = selectChoices[0];
		int rightID = selectChoices[1];
		Vector2 leftPos = {350, 300};
		Vector2 rightPos = {750, 300};
		levelupIcons[leftID]->SetPosition(leftPos);
		levelupIcons[leftID]->SetColor(cursorIndex == 0 ? Vector4(1, 1, 0, 1) : Vector4(1, 1, 1, 1));
		levelupIcons[leftID]->Update();
		levelupIcons[leftID]->Draw();
		levelupIcons[rightID]->SetPosition(rightPos);
		levelupIcons[rightID]->SetColor(cursorIndex == 1 ? Vector4(1, 1, 0, 1) : Vector4(1, 1, 1, 1));
		levelupIcons[rightID]->Update();
		levelupIcons[rightID]->Draw();
	}
}
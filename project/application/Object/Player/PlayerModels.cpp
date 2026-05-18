#include "PlayerModels.h"
#include "Function.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "imgui.h"
#include <cmath>
#include <numbers>

PlayerModels::PlayerModels() : state_(StateM::idle) {};
PlayerModels::~PlayerModels() {};

void PlayerModels::SetCharacterType(const std::string& characterName) {
	if (!playableManager_.ChangePlayable(characterName)) {
		playableManager_.ChangePlayable("Sizuku");
	}
	currentCharacter_ = playableManager_.GetCurrentPlayable();
}

void PlayerModels::Initialize() {
	playableManager_.Initialize();
	currentCharacter_ = playableManager_.GetCurrentPlayable();
}

void PlayerModels::Update() {
	if (!currentCharacter_) {
		return;
	}

	currentCharacter_->SetTransform(player_);
	std::string animationName = "Idle";
	switch (state_) {
	case PlayerModels::idle:
		animationName = "Idle";
		break;
	case PlayerModels::walk:
		animationName = "Walk";
		break;
	case PlayerModels::attack1:
		animationName = "Attack1";
		break;
	case PlayerModels::attack2:
		animationName = "Attack2";
		break;
	case PlayerModels::attack3:
		animationName = "Attack3";
		break;
	case PlayerModels::attack4:
		animationName = "Attack4";
		break;
	case PlayerModels::fallingAttack:
		animationName = "FallAttack";
		break;
	case PlayerModels::skillAttack:
		animationName = "SkillAttack";
		break;
	case PlayerModels::damage:
		animationName = "damage";
		break;
	default:
		break;
	}

	currentCharacter_->SetAnimation(animationName);
	currentCharacter_->SetCamera(camera_);
	currentCharacter_->Update();

#ifdef USE_IMGUI
	if (!ImGui::Begin("Player Parts Adjust")) {
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Player Root")) {
		ImGui::DragFloat3("Player Pos", &player_.translate.x, 0.01f);
		ImGui::DragFloat3("Player Rot", &player_.rotate.x, 0.01f);
		ImGui::DragFloat3("Player Scale", &player_.scale.x, 0.01f);
	}

	ImGui::End();
#endif
}

void PlayerModels::Draw() {

	Object3dCommon::GetInstance()->DrawCommonSkinningToon();
	currentCharacter_->Draw();
	Object3dCommon::GetInstance()->DrawCommonSkinningToonOutline();
	currentCharacter_->Draw();
	Object3dCommon::GetInstance()->EndOutlineDraw();
}
std::optional<Matrix4x4> PlayerModels::GetJointWorldMatrix(const std::string& jointName) const {
	if (!currentCharacter_) {
		return std::nullopt;
	}
	return currentCharacter_->GetJointWorldMatrix(jointName);
}

bool PlayerModels::IsAttackAnimationFinished() const { return currentCharacter_ ? currentCharacter_->IsAnimationFinished() : false; }

Object3d* PlayerModels::GetCharacterObject3d() { return currentCharacter_ ? currentCharacter_->GetObject3d() : nullptr; }

Attribute PlayerModels::GetCurrentAttribute() const { return currentCharacter_ ? currentCharacter_->GetAttribute() : Attribute::None; }
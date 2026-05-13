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

void PlayerModels::SetCharacterType(CharacterType characterType) {
	currentCharacterType_ = characterType;
	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		currentCharacter_ = sizuku_.get();
		break;
	case CharacterType::Mei:
		currentCharacter_ = mei_.get();
		break;
	case CharacterType::Yuzuki:
		currentCharacter_ = yuzuki_.get();
		break;
	case CharacterType::Arte:
		currentCharacter_ = arte_.get();
		break;
	default:
		currentCharacter_ = sizuku_.get();
		break;
	}
}

void PlayerModels::Initialize() {
	sizuku_ = std::make_unique<Sizuku>();
	mei_ = std::make_unique<Mei>();
	yuzuki_ = std::make_unique<Yuzuki>();
	arte_ = std::make_unique<Arte>();
	sizuku_->Initialize();
	mei_->Initialize();
	yuzuki_->Initialize();
	arte_->Initialize();
	SetCharacterType(CharacterType::Sizuku);
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

	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		sizuku_->SetAnimation(animationName);
		sizuku_->SetCamera(camera_);
		sizuku_->Update();
		break;
	case CharacterType::Mei:
		mei_->SetAnimation(animationName);
		mei_->SetCamera(camera_);
		mei_->Update();
		break;
	case CharacterType::Yuzuki:
		yuzuki_->SetAnimation(animationName);
		yuzuki_->SetCamera(camera_);
		yuzuki_->Update();
		break;
	case CharacterType::Arte:
		arte_->SetAnimation(animationName);
		arte_->SetCamera(camera_);
		arte_->Update();
		break;
	default:
		break;
	}

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
	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		sizuku_->Draw();
		break;
	case CharacterType::Mei:
		mei_->Draw();
		break;
	case CharacterType::Yuzuki:
		yuzuki_->Draw();
		break;
	case CharacterType::Arte:
		arte_->Draw();
		break;
	default:
		break;
	}
}
std::optional<Matrix4x4> PlayerModels::GetJointWorldMatrix(const std::string& jointName) const {
	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		return sizuku_->GetJointWorldMatrix(jointName);
	case CharacterType::Mei:
		return mei_->GetJointWorldMatrix(jointName);
	case CharacterType::Yuzuki:
		return yuzuki_->GetJointWorldMatrix(jointName);
	case CharacterType::Arte:
		return arte_->GetJointWorldMatrix(jointName);
	default:
		return std::nullopt;
	}
}

bool PlayerModels::IsAttackAnimationFinished() const {
	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		return sizuku_ ? sizuku_->IsAnimationFinished() : false;
	case CharacterType::Mei:
		return mei_ ? mei_->IsAnimationFinished() : false;
	case CharacterType::Yuzuki:
		return yuzuki_ ? yuzuki_->IsAnimationFinished() : false;
	case CharacterType::Arte:
		return arte_ ? arte_->IsAnimationFinished() : false;
	default:
		return false;
	}
}

Object3d* PlayerModels::GetCharacterObject3d() {
	switch (currentCharacterType_) {
	case CharacterType::Sizuku:
		return sizuku_ ? sizuku_->GetObject3d() : nullptr;
	case CharacterType::Mei:
		return mei_ ? mei_->GetObject3d() : nullptr;
	case CharacterType::Yuzuki:
		return yuzuki_ ? yuzuki_->GetObject3d() : nullptr;
	case CharacterType::Arte:
		return arte_ ? arte_->GetObject3d() : nullptr;
	default:
		return nullptr;
	}
}
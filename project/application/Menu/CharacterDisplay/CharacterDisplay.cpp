#include "CharacterDisplay.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "SpriteCommon.h"
#include <numbers>
#include <imgui.h>
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
void CharacterDisplay::Initialize() {
	if (!camera_) {
	camera_ = std::make_unique<Camera>();
	}
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	if (!sizukuObject_) {
	sizukuObject_ = std::make_unique<Sizuku>();
	sizukuObject_->Initialize();
	}
	
	sizukuObject_->SetCamera(camera_.get());
	sizukuObject_->SetAnimation("InitIdle");
	sizukuObject_->SetTransform(characterTransform_);
	sizukuObject_->Update();

	if (!skyDome_) {
	skyDome_ = std::make_unique<CharacterDisplaySkyDome>();
		skyDome_->Initialize(camera_.get());
	}
	menuText_ = std::make_unique<CharacterDisplayMenuText>();
	menuText_->Initialize();

	status_ = std::make_unique<CharacterDisplayStatus>();
	status_->Initialize();
	weapon_ = std::make_unique<CharacterDisplayWeapon>();
	weapon_->Initialize();
	equip_ = std::make_unique<CharacterDisplayEquip>();
	equip_->Initialize();
	skilltree_ = std::make_unique<CharacterDisplaySkilltree>();
	skilltree_->Initialize();
	reinforcement_ = std::make_unique<CharacterDisplayReinforcement>();
	reinforcement_->Initialize();
	profile_ = std::make_unique<CharacterDisplayProfile>();
	profile_->Initialize();
}

void CharacterDisplay::Update() {
	if (!isActive_ || !sizukuObject_ || !camera_) {
		Input::GetInstance()->SetIsCursorStability(true);
		Input::GetInstance()->SetIsCursorVisible(false);
		return;
	}
	Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight);
	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
	camera_->SetTransform(cameraTransform_);
	camera_->Update();
	skyDome_->Update();

#ifdef USE_IMGUI
	if (ImGui::Begin("CharacterDisplay")) {
		if (ImGui::TreeNode("Character Transform")) {
			ImGui::DragFloat3("Character Scale", &characterTransform_.scale.x, 0.01f);
			ImGui::DragFloat3("Character Rotate", &characterTransform_.rotate.x, 0.01f);
			ImGui::DragFloat3("Character Translate", &characterTransform_.translate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Display Camera Transform")) {
			ImGui::DragFloat3("Camera Scale", &cameraTransform_.scale.x, 0.01f);
			ImGui::DragFloat3("Camera Rotate", &cameraTransform_.rotate.x, 0.01f);
			ImGui::DragFloat3("Camera Translate", &cameraTransform_.translate.x, 0.01f);
			ImGui::TreePop();
		}
	}
	ImGui::End();
#endif // USE_IMGUI
	sizukuObject_->SetCamera(camera_.get());
	sizukuObject_->SetTransform(characterTransform_);
	sizukuObject_->Update();

	switch (selectMenuType_) {
	case CharacterDisplayMenuType::STATUS:
		if (PlayCommand::GetDownUI()) {
			selectMenuType_ = CharacterDisplayMenuType::WEAPON;
		}
		status_->Update();
		break;
	case CharacterDisplayMenuType::WEAPON:
		if (PlayCommand::GetUpUI()) {
			selectMenuType_ = CharacterDisplayMenuType::STATUS;
		}
		if (PlayCommand::GetDownUI()) {
			selectMenuType_ = CharacterDisplayMenuType::EQUIP;
		}
		weapon_->Update();
		break;
	case CharacterDisplayMenuType::EQUIP:
		if (PlayCommand::GetUpUI()) {
			selectMenuType_ = CharacterDisplayMenuType::WEAPON;
		}
		if (PlayCommand::GetDownUI()) {
			selectMenuType_ = CharacterDisplayMenuType::SKILLTREE;
		}
		equip_->Update();
		break;
	case CharacterDisplayMenuType::SKILLTREE:
		if (PlayCommand::GetUpUI()) {
			selectMenuType_ = CharacterDisplayMenuType::EQUIP;
		}
		if (PlayCommand::GetDownUI()) {
			selectMenuType_ = CharacterDisplayMenuType::REINFORCEMENT;
		}
		skilltree_->Update();
		break;
	case CharacterDisplayMenuType::REINFORCEMENT:
		if (PlayCommand::GetUpUI()) {
			selectMenuType_ = CharacterDisplayMenuType::SKILLTREE;
		}
		if (PlayCommand::GetDownUI()) {
			selectMenuType_ = CharacterDisplayMenuType ::PROFILE;
		}
		reinforcement_->Update();
		break;
	case CharacterDisplayMenuType::PROFILE:
		if (PlayCommand::GetUpUI()) {
			selectMenuType_ = CharacterDisplayMenuType::REINFORCEMENT;
		}
		profile_->Update();
		break;
	default:
		break;
	}


	menuText_->SetMenuType(selectMenuType_);
	menuText_->Update();
}

void CharacterDisplay::Draw() {
	if (!isActive_ || !sizukuObject_) {
		return;
	}

	Object3dCommon::GetInstance()->DrawCommon();
	skyDome_->Draw();

	switch (selectMenuType_) {
	case CharacterDisplayMenuType::STATUS:
		status_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		sizukuObject_->Draw();
		break;
	case CharacterDisplayMenuType::WEAPON:
		weapon_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		sizukuObject_->Draw();
		break;
	case CharacterDisplayMenuType::EQUIP:
		equip_->Draw();
		break;
	case CharacterDisplayMenuType::SKILLTREE:
		skilltree_->Draw();
		break;
	case CharacterDisplayMenuType::REINFORCEMENT:
		reinforcement_->Draw();
		break;
	case CharacterDisplayMenuType::PROFILE:
		profile_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		sizukuObject_->Draw();
		break;
	default:
		break;
	}

	SpriteCommon::GetInstance()->DrawCommon();
	menuText_->Draw();
}
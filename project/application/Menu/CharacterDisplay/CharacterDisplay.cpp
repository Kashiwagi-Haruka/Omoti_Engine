#include "CharacterDisplay.h"
#include "Input.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "PlayCommand/PlayCommand.h"
#include "SpriteCommon.h"
#include "Team/Team.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "TextureManager.h"
#include <algorithm>
#include <imgui.h>
#include <numbers>
void CharacterDisplay::Initialize(const Team& team) {
	if (!camera_) {
		camera_ = std::make_unique<Camera>();
	}
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	if (!skyDome_) {
		skyDome_ = std::make_unique<CharacterDisplaySkyDome>();
		skyDome_->Initialize(camera_.get());
	}
	menuText_ = std::make_unique<CharacterDisplayMenuText>();
	menuText_->Initialize();

	characterDisplayIcon_ = std::make_unique<CharacterDisplayIcon>();
	characterDisplayIcon_->Initialize(team);

	characterNameFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(characterNameFontHandle_, 36, 36);
	characterNameText_.Initialize(characterNameFontHandle_);
	characterNameText_.SetSize({360.0f, 60.0f});
	characterNameText_.SetPosition({640.0f, 80.0f});
	characterNameText_.SetAlign(TextAlign::Center);

	switchLeftGuideSprite_ = std::make_unique<Sprite>();
	switchRightGuideSprite_ = std::make_unique<Sprite>();

	uint32_t switchHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/CharacterDIsplay/LeftRight.png");

	switchLeftGuideSprite_->Initialize(switchHandle);

	characterSwitchGuideText_.Initialize(characterNameFontHandle_);
	characterSwitchGuideText_.SetSize({480.0f, 40.0f});
	characterSwitchGuideText_.SetPosition({640.0f, 130.0f});
	characterSwitchGuideText_.SetAlign(TextAlign::Center);
	characterSwitchGuideText_.SetColor({1.0f, 1.0f, 1.0f, 0.8f});
	characterSwitchGuideText_.SetString(U"A/D:キャラクター切り替え");
	characterSwitchGuideText_.UpdateLayout(false);

	RebuildOwnedCharacters(team);
	ChangeDisplayedCharacter(selectedCharacterIndex_);

	if (characterDisplayIcon_) {
		characterDisplayIcon_->Update(selectedCharacterIndex_);
	}

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

void CharacterDisplay::RebuildOwnedCharacters(const Team& team) {
	ownedPlayableNames_.clear();
	ownedDisplayNames_.clear();
	for (int i = 0; i < team.GetOwnedCharacterCount(); ++i) {
		ownedPlayableNames_.push_back(team.GetPlayableNameByIndex(i));
		ownedDisplayNames_.push_back(team.GetCharacterNameByIndex(i));
	}
	if (ownedPlayableNames_.empty()) {
		ownedPlayableNames_.push_back("Sizuku");
		ownedDisplayNames_.push_back(U"Sizuku");
	}
	selectedCharacterIndex_ = std::min(selectedCharacterIndex_, ownedPlayableNames_.size() - 1);
}

void CharacterDisplay::ChangeDisplayedCharacter(size_t characterIndex) {
	if (ownedPlayableNames_.empty()) {
		return;
	}
	selectedCharacterIndex_ = characterIndex % ownedPlayableNames_.size();
	if (!playableManager_.ChangePlayable(ownedPlayableNames_[selectedCharacterIndex_])) {
		playableManager_.ChangePlayable("Sizuku");
	}
	currentPlayable_ = playableManager_.GetCurrentPlayable();
	if (!currentPlayable_) {
		return;
	}
	currentPlayable_->SetCamera(camera_.get());
	currentPlayable_->SetAnimation("InitIdle");
	currentPlayable_->SetTransform(characterTransform_);
	currentPlayable_->Update();
	if (skilltree_) {
		skilltree_->SetSkilltreeType(currentPlayable_->GetSkilltreeType());
		skilltree_->SetArcana(currentPlayable_->GetArcana());
	}
	if (reinforcement_) {
		reinforcement_->SetReinforcementCount(currentPlayable_->GetReinforcement());
	}
	characterNameText_.SetString(ownedDisplayNames_[selectedCharacterIndex_]);
	characterNameText_.UpdateLayout(false);
}

void CharacterDisplay::Update() {
	if (!isActive_ || !currentPlayable_ || !camera_) {
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
	if (Input::GetInstance()->TriggerKey(DIK_A) && !ownedPlayableNames_.empty()) {
		const size_t prevIndex = (selectedCharacterIndex_ + ownedPlayableNames_.size() - 1) % ownedPlayableNames_.size();
		ChangeDisplayedCharacter(prevIndex);
	}
	if (Input::GetInstance()->TriggerKey(DIK_D) && !ownedPlayableNames_.empty()) {
		const size_t nextIndex = (selectedCharacterIndex_ + 1) % ownedPlayableNames_.size();
		ChangeDisplayedCharacter(nextIndex);
	}

	currentPlayable_->SetCamera(camera_.get());
	currentPlayable_->SetTransform(characterTransform_);
	currentPlayable_->Update();
	characterNameText_.UpdateLayout(false);

	if (characterDisplayIcon_) {
		characterDisplayIcon_->Update(selectedCharacterIndex_);
	}

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
	if (!isActive_ || !currentPlayable_) {
		return;
	}

	Object3dCommon::GetInstance()->DrawCommon();
	skyDome_->Draw();

	switch (selectMenuType_) {
	case CharacterDisplayMenuType::STATUS:
		status_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	case CharacterDisplayMenuType::WEAPON:
		weapon_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	case CharacterDisplayMenuType::EQUIP:
		equip_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	case CharacterDisplayMenuType::SKILLTREE:
		skilltree_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	case CharacterDisplayMenuType::REINFORCEMENT:
		reinforcement_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	case CharacterDisplayMenuType::PROFILE:
		profile_->Draw();
		Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::SkinningToon);
		currentPlayable_->Draw();
		break;
	default:
		break;
	}

	SpriteCommon::GetInstance()->DrawCommon();
	if (characterDisplayIcon_) {
		characterDisplayIcon_->Draw();
	}
	characterNameText_.Draw();
	characterSwitchGuideText_.Draw();

	menuText_->Draw();
}
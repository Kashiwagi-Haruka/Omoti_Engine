#include "TeamDisplay.h"
#include "Function.h"
#include "Input.h"
#include "Object3d/Object3dCommon.h"
#include "SpriteCommon.h"
#include "Team/Team.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "TextureManager.h"

void TeamDisplay::Initialize(const Team& team) {
	whiteTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	hudFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(hudFontHandle_, 28, 28);

	camera_ = std::make_unique<Camera>();
	camera_->SetTransform({
	    .scale{1.0f, 1.0f, 1.0f  },
	    .rotate{0.0f, 0.0f, 0.0f  },
	    .translate{0.0f, 0.0f, -10.0f},
	});
	camera_->Update();

	teamBackgroundPlane_ = std::make_unique<Primitive>();
	teamBackgroundPlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Team/Background.png");
	teamBackgroundPlane_->SetEnableLighting(false);
	teamBackgroundPlane_->SetTransform({
	    {16.0f * 2.0f, 9.0f * 2.0f, 1.0f},
        {0.0f,         0.0f,        0.0f},
        {0.0f,         0.0f,        0.0f}
    });
	teamBackgroundPlane_->SetCamera(camera_.get());
	teamBackgroundPlane_->Update();

	teamFormationText_.Initialize(hudFontHandle_);
	teamFormationText_.SetSize({400.0f, 60.0f});
	teamFormationText_.SetPosition({10.0f, 40.0f});
	teamFormationText_.SetAlign(TextAlign::Left);
	teamFormationText_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	teamFormationText_.SetString(U"チーム編成");
	teamFormationText_.UpdateLayout(false);

	ownedCharacterIconHandles_.clear();
	for (int i = 0; i < team.GetOwnedCharacterCount(); ++i) {
		ownedCharacterIconHandles_.push_back(team.GetOwnedCharacterIconHandle(i));
	}

	const std::array<Vector4, kMaxMembersCount> teamDisplayModelColors = {
	    Vector4{1.0f,            1.0f,            1.0f,            1.0f},
	    Vector4{255.0f / 255.0f, 106.0f / 255.0f, 145.0f / 255.0f, 1.0f},
	    Vector4{93.0f / 255.0f,  236.0f / 255.0f, 203.0f / 255.0f, 1.0f},
	    Vector4{207.0f / 255.0f, 156.0f / 255.0f, 236.0f / 255.0f, 1.0f},
	};
	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamDisplayModels_[i] = std::make_unique<Object3d>();
		teamDisplayModels_[i]->SetModel("sizuku");
		teamDisplayModels_[i]->Initialize();
		teamDisplayModels_[i]->SetCamera(camera_.get());
		teamDisplayModels_[i]->SetColor(teamDisplayModelColors[i]);
		teamDisplayModels_[i]->SetShininess(20.0f);
		teamDisplayModels_[i]->SetOutlineWidth(2.0f);
		teamDisplayModels_[i]->SetOutlineColor({99.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f, 1.0f});
	}

	inventoryIcons_.clear();
	for (size_t i = 0; i < ownedCharacterIconHandles_.size(); ++i) {
		auto icon = std::make_unique<Sprite>();
		icon->Initialize(ownedCharacterIconHandles_[i]);
		icon->SetScale(iconSize_);
		icon->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		const float x = inventoryPanelPos_.x + 22.0f + (iconSize_.x + 18.0f) * static_cast<float>(i);
		const float y = inventoryPanelPos_.y + 24.0f;
		inventoryIconPositions_[i] = {x, y};
		icon->SetPosition(inventoryIconPositions_[i]);
		icon->Update();
		inventoryIcons_.push_back(std::move(icon));
	}

	inventoryBg_ = std::make_unique<Sprite>();
	inventoryBg_->Initialize(whiteTextureHandle_);
	inventoryBg_->SetPosition(inventoryPanelPos_);
	inventoryBg_->SetScale(inventoryPanelSize_);
	inventoryBg_->SetColor({0.08f, 0.08f, 0.12f, 0.92f});
	inventoryBg_->Update();

	confirmButton_ = std::make_unique<Sprite>();
	confirmButton_->Initialize(whiteTextureHandle_);
	confirmButton_->SetPosition(confirmPos_);
	confirmButton_->SetScale(confirmSize_);
	confirmButton_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	confirmButton_->Update();

	confirmButtonText_.Initialize(hudFontHandle_);
	confirmButtonText_.SetSize(confirmSize_);
	confirmButtonText_.SetAlign(TextAlign::Center);
	confirmButtonText_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	confirmButtonText_.SetString(U"確定");
	confirmButtonText_.SetPosition({confirmPos_.x + confirmSize_.x * 0.5f, confirmPos_.y});
	confirmButtonText_.UpdateLayout(false);

	candidatePreview_ = std::make_unique<Sprite>();
	candidatePreview_->Initialize(ownedCharacterIconHandles_[selectedInventoryIndex_]);
	candidatePreview_->SetPosition(previewPos_);
	candidatePreview_->SetScale(previewSize_);
	candidatePreview_->SetColor({1.0f, 1.0f, 1.0f, 0.95f});
	candidatePreview_->Update();

	draggingIcon_ = std::make_unique<Sprite>();
	draggingIcon_->Initialize(ownedCharacterIconHandles_[selectedInventoryIndex_]);
	draggingIcon_->SetScale(iconSize_);
	draggingIcon_->SetColor({1.0f, 1.0f, 1.0f, 0.78f});
	draggingIcon_->Update();

	memberSelectionMarker_ = std::make_unique<Sprite>();
	memberSelectionMarker_->Initialize(whiteTextureHandle_);
	memberSelectionMarker_->SetScale({slotSize_.x + 10.0f, slotSize_.y + 10.0f});
	memberSelectionMarker_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	memberSelectionMarker_->SetPosition({confirmPos_.x + confirmSize_.x * 0.5f - confirmSize_.x * 2.0f, confirmPos_.y});
	memberSelectionMarker_->Update();

	formationTransitionText_.Initialize(hudFontHandle_);
	formationTransitionText_.SetSize({400.0f, 60.0f});
	formationTransitionText_.SetAlign(TextAlign::Center);
	formationTransitionText_.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	formationTransitionText_.SetPosition(memberSelectionMarker_->GetPosition());
	formationTransitionText_.SetString(U"編成変更");
	formationTransitionText_.UpdateLayout(false);

	inventorySelectionMarker_ = std::make_unique<Sprite>();
	inventorySelectionMarker_->Initialize(whiteTextureHandle_);
	inventorySelectionMarker_->SetScale({iconSize_.x + 10.0f, iconSize_.y + 10.0f});
	inventorySelectionMarker_->SetColor({1.0f, 0.85f, 0.2f, 0.35f});
	inventorySelectionMarker_->SetPosition({inventoryIconPositions_[selectedInventoryIndex_].x - 5.0f, inventoryIconPositions_[selectedInventoryIndex_].y - 5.0f});
	inventorySelectionMarker_->Update();

	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamSlotSprites_[i] = std::make_unique<Sprite>();
		teamSlotSprites_[i]->Initialize(whiteTextureHandle_);
		teamSlotSprites_[i]->SetScale(slotSize_);
		teamSlotSprites_[i]->SetColor({0.15f, 0.15f, 0.2f, 0.9f});

		teamMemberIcons_[i] = std::make_unique<Sprite>();
		teamMemberIcons_[i]->Initialize(ownedCharacterIconHandles_.empty() ? whiteTextureHandle_ : ownedCharacterIconHandles_[0]);
		teamMemberIcons_[i]->SetScale(iconSize_);
		teamMemberIcons_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

		noMemberPlanes_[i] = std::make_unique<Primitive>();
		noMemberPlanes_[i]->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Team/NoMember.png");
		noMemberPlanes_[i]->SetEnableLighting(false);
		noMemberPlanes_[i]->SetTransform({
		    noMemberPlaneScale_, {0.0f, 0.0f, 0.0f},
             teamDisplayPositions_[i]
        });
		noMemberPlanes_[i]->SetCamera(camera_.get());
		noMemberPlanes_[i]->Update();

		slotPositions_[i] = {40.0f + (slotSize_.x + 18.0f) * i, 40.0f};
		teamSlotSprites_[i]->SetPosition(slotPositions_[i]);
		teamSlotSprites_[i]->Update();
	}
	for (int i = 0; i < kMaxMembersCount; ++i) {
		const int characterIndex = team.GetMemberCharacterIndex(i);
		if (team.GetHasMember(i) && characterIndex >= 0 && characterIndex < static_cast<int>(ownedCharacterIconHandles_.size())) {
			teamMemberIcons_[i]->Initialize(ownedCharacterIconHandles_[characterIndex]);
		}
	}
	cursorPos_ = {Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()};
	cursorSprite_ = std::make_unique<Sprite>();
	uint32_t cursorTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Cursor.png");
	cursorSprite_->Initialize(cursorTextureHandle);
	cursorSprite_->SetPosition(cursorPos_);
	cursorSprite_->SetScale({100.0f, 100.0f});
	cursorSprite_->Update();
}

void TeamDisplay::Unload() {
	ownedCharacterIconHandles_.clear();
	for (auto& model : teamDisplayModels_) {
		model.reset();
	}
	for (auto& plane : noMemberPlanes_) {
		plane.reset();
	}
	for (auto& sprite : teamSlotSprites_) {
		sprite.reset();
	}
	for (auto& sprite : teamMemberIcons_) {
		sprite.reset();
	}
	inventoryIcons_.clear();
	inventoryBg_.reset();
	confirmButton_.reset();
	candidatePreview_.reset();
	draggingIcon_.reset();
	inventorySelectionMarker_.reset();
	memberSelectionMarker_.reset();
	teamBackgroundPlane_.reset();
	cursorSprite_.reset();
	camera_.reset();
	isCandidateSelected_ = false;
	isDraggingInventoryIcon_ = false;
	isMemberSelectionActive_ = false;
}

void TeamDisplay::Update(Team& team) {
	camera_->Update();
	teamBackgroundPlane_->SetCamera(camera_.get());
	teamBackgroundPlane_->Update();
	for (int i = 0; i < kMaxMembersCount; ++i) {
		noMemberPlanes_[i]->SetCamera(camera_.get());
		noMemberPlanes_[i]->Update();
	}
	Input::GetInstance()->SetIsCursorVisible(false);
	Input::GetInstance()->SetIsCursorStability(false);
	UpdatePartyUI(team);
	cursorSprite_->SetPosition(cursorPos_);
	cursorSprite_->Update();
}

void TeamDisplay::UpdatePartyUI(Team& team) {
	cursorPos_ += {Input::GetInstance()->GetMouseMove().x + Input::GetInstance()->GetJoyStickLX() * 10.0f, Input::GetInstance()->GetMouseMove().y + Input::GetInstance()->GetJoyStickLY() * -10.0f};
	const Vector2 memberSelectionMarkerPos = memberSelectionMarker_->GetPosition();
	const Vector2 memberSelectionMarkerSize = memberSelectionMarker_->GetScale();
	const bool isHoveringMemberSelectionMarker = IsInsideRect(cursorPos_, memberSelectionMarkerPos, memberSelectionMarkerSize);
	
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonB)) {
		if (isMemberSelectionActive_) {
			isMemberSelectionActive_ = false;
		}
	}
	
	
	if (isHoveringMemberSelectionMarker && (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA))) {
		isMemberSelectionActive_ = true;
	}
	memberSelectionMarker_->SetColor(isHoveringMemberSelectionMarker ? Vector4{0.95f, 0.8f, 0.3f, 1.0f} : Vector4{1.0f, 1.0f, 1.0f, 1.0f});
	hoveredInventoryIndex_ = -1;
	dropTargetSlotIndex_ = -1;
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (IsInsideRect(cursorPos_, slotPositions_[i], slotSize_)) {
			dropTargetSlotIndex_ = i;
			if ((Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA))) {
				team.SetActiveSlot(i);
			}
		}
	}

	for (size_t i = 0; i < inventoryIcons_.size(); ++i) {
		if (IsInsideRect(cursorPos_, inventoryIconPositions_[i], iconSize_)) {
			hoveredInventoryIndex_ = static_cast<int>(i);
			break;
		}
	}

	if (hoveredInventoryIndex_ >= 0 && (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA))) {
		selectedInventoryIndex_ = hoveredInventoryIndex_;
		isCandidateSelected_ = true;
		isDraggingInventoryIcon_ = true;
		draggingInventoryIndex_ = hoveredInventoryIndex_;
		candidatePreview_->Initialize(ownedCharacterIconHandles_[selectedInventoryIndex_]);
		candidatePreview_->Update();
		draggingIcon_->Initialize(ownedCharacterIconHandles_[draggingInventoryIndex_]);
	}

	if (isDraggingInventoryIcon_) {
		if (draggingInventoryIndex_ >= 0 && draggingInventoryIndex_ < static_cast<int>(ownedCharacterIconHandles_.size())) {
			draggingIcon_->SetPosition({cursorPos_.x - iconSize_.x * 0.5f, cursorPos_.y - iconSize_.y * 0.5f});
			draggingIcon_->SetColor({1.0f, 1.0f, 1.0f, dropTargetSlotIndex_ >= 0 ? 0.95f : 0.78f});
			draggingIcon_->Update();
		}
		if ((Input::GetInstance()->ReleaseMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->ReleaseButton(Input::PadButton::kButtonA))) {
			if (dropTargetSlotIndex_ >= 0) {
				team.AssignCharacterToSlot(dropTargetSlotIndex_, draggingInventoryIndex_);
				teamMemberIcons_[dropTargetSlotIndex_]->Initialize(ownedCharacterIconHandles_[draggingInventoryIndex_]);
			}
			isDraggingInventoryIcon_ = false;
			draggingInventoryIndex_ = -1;
		}
	}

	const bool isHoveringConfirm = IsInsideRect(cursorPos_, confirmPos_, confirmSize_);
	const bool isConfirmTriggered = isHoveringConfirm && ((Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA)) ||
	                                                      Input::GetInstance()->TriggerKey(DIK_SPACE));
	if (isConfirmTriggered) {
		team.AssignCharacterToSlot(team.GetActiveSlot(), selectedInventoryIndex_);
		teamMemberIcons_[team.GetActiveSlot()]->Initialize(ownedCharacterIconHandles_[selectedInventoryIndex_]);
		isMemberSelectionActive_ = false;
	}

	confirmButton_->SetColor(isHoveringConfirm ? Vector4{0.95f, 0.8f, 0.3f, 1.0f} : Vector4{0.2f, 0.55f, 0.9f, 0.95f});

	for (int i = 0; i < kMaxMembersCount; ++i) {
		Vector4 slotColor = {0.15f, 0.15f, 0.2f, 0.9f};
		if (isDraggingInventoryIcon_ && dropTargetSlotIndex_ == i) {
			slotColor = {0.95f, 0.8f, 0.3f, 1.0f};
		} else if (team.GetActiveSlot() == i && team.GetHasMember(i)) {
			slotColor = {0.2f, 0.85f, 0.35f, 1.0f};
		}
		teamSlotSprites_[i]->SetColor(slotColor);
		teamSlotSprites_[i]->Update();
		if (team.GetHasMember(i)) {
			Vector2 memberIconPos{slotPositions_[i].x + 8.0f, slotPositions_[i].y + 8.0f};
			teamMemberIcons_[i]->SetPosition(memberIconPos);
			teamMemberIcons_[i]->Update();
		}
	}

	for (size_t i = 0; i < inventoryIcons_.size(); ++i) {
		Vector4 color = {1, 1, 1, 1};
		if (static_cast<int>(i) == hoveredInventoryIndex_) {
			color = {1.0f, 0.95f, 0.7f, 1.0f};
		}
		if (isDraggingInventoryIcon_ && static_cast<int>(i) == draggingInventoryIndex_) {
			color.w = 0.55f;
		}
		inventoryIcons_[i]->SetColor(color);
		inventoryIcons_[i]->Update();
	}

	const int markerIndex = hoveredInventoryIndex_ >= 0 ? hoveredInventoryIndex_ : selectedInventoryIndex_;
	if (markerIndex >= 0 && markerIndex < static_cast<int>(inventoryIcons_.size())) {
		Vector2 markerPos = inventoryIconPositions_[markerIndex];
		markerPos.x -= 5.0f;
		markerPos.y -= 5.0f;
		inventorySelectionMarker_->SetPosition(markerPos);
		inventorySelectionMarker_->Update();
	}

	inventoryBg_->Update();
	confirmButton_->Update();
	confirmButtonText_.Update(false);
	candidatePreview_->Update();
	formationTransitionText_.Update(false);
	teamFormationText_.Update(false);
}

void TeamDisplay::Draw(const Team& team) {
	Object3dCommon::GetInstance()->DrawCommon();
	teamBackgroundPlane_->Draw();
	if (!isMemberSelectionActive_) {
		DrawTeamDisplayMembers(team);
	}

	SpriteCommon::GetInstance()->DrawCommon();

	memberSelectionMarker_->Draw();
	confirmButton_->Draw();
	formationTransitionText_.Draw();
	teamFormationText_.Draw();
	confirmButtonText_.Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	if (isMemberSelectionActive_) {
		
		for (int i = 0; i < kMaxMembersCount; ++i) {
			teamSlotSprites_[i]->Draw();
			if (team.GetHasMember(i)) {
				teamMemberIcons_[i]->Draw();
			}
		}
		inventoryBg_->Draw();
		inventorySelectionMarker_->Draw();
		for (auto& icon : inventoryIcons_) {
			icon->Draw();
		}
		if (isCandidateSelected_) {
			candidatePreview_->Draw();
		}
		if (isDraggingInventoryIcon_) {
			draggingIcon_->Draw();
		}
	}
	cursorSprite_->Draw();
}
void TeamDisplay::DrawTeamDisplayMembers(const Team& team) {
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (!team.GetHasMember(i)) {
			noMemberPlanes_[i]->Draw();
			continue;
		}

		const int characterIndex = team.GetMemberCharacterIndex(i);
		if (characterIndex < 0 || characterIndex >= static_cast<int>(teamDisplayModels_.size())) {
			noMemberPlanes_[i]->Draw();
			continue;
		}

		teamDisplayModels_[characterIndex]->SetTransform({
		    teamDisplayModelScale_, {0.0f, 3.14159265f, 0.0f},
             teamDisplayPositions_[i]
        });
		teamDisplayModels_[characterIndex]->SetCamera(camera_.get());
		teamDisplayModels_[characterIndex]->Update();
		teamDisplayModels_[characterIndex]->Draw();
	}
}
bool TeamDisplay::IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const {
	return point.x >= pos.x && point.x <= pos.x + size.x && point.y >= pos.y && point.y <= pos.y + size.y;
}
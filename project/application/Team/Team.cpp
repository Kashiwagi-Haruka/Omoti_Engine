#include "Team.h"
#include "Input.h"
#include "Object/Characters/Playable/Individual/Sizuku/Sizuku.h"
#include "Object/Characters/Playable/Individual/Mei/Mei.h"
#include "Object/Characters/Playable/Individual/Arte/Arte.h"
#include "Object/Characters/Playable/Individual/Yuzuki/Yuzuki.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
#include "TextureManager.h"
#include "Object3d/Object3dCommon.h"
#include "SpriteCommon.h"

void Team::Initialize() {
	whiteTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	const uint32_t sizukuIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Sizuku.png");
	const uint32_t meiIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Mei.png");
	const uint32_t arteIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Arte.png");
	const uint32_t yuzukiIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Yuzuki.png");
	hudFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(hudFontHandle_, 28, 28);

	camera_ = std::make_unique<Camera>();
	camera_->SetTransform({
		.scale{1.0f, 1.0f, 1.0f},
		.rotate{0.0f, 0.0f, 0.0f},
		.translate{0.0f, 0.0f, -10.0f},
	});
	camera_->Update();

	teamBackgroundPlane_ = std::make_unique<Primitive>();
	teamBackgroundPlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Team/Background.png");
	teamBackgroundPlane_->SetEnableLighting(false);
	teamBackgroundPlane_->SetTransform({{16.0f*2.0f,9.0f*2.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}});
	teamBackgroundPlane_->SetCamera(camera_.get());
	teamBackgroundPlane_->Update();

	ownedCharacters_.clear();
	ownedCharacterIconHandles_.clear();
	ownedCharacters_.push_back(std::make_unique<Sizuku>());
	ownedCharacterIconHandles_.push_back(sizukuIconHandle);
	ownedCharacters_.push_back(std::make_unique<Mei>());
	ownedCharacterIconHandles_.push_back(meiIconHandle);
	ownedCharacters_.push_back(std::make_unique<Arte>());
	ownedCharacterIconHandles_.push_back(arteIconHandle);
	ownedCharacters_.push_back(std::make_unique<Yuzuki>());
	ownedCharacterIconHandles_.push_back(yuzukiIconHandle);

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
	confirmButton_->SetColor({0.2f, 0.55f, 0.9f, 0.95f});
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
	memberSelectionMarker_->SetPosition({confirmPos_.x + confirmSize_.x*0.5f-confirmSize_.x*2.0f,confirmPos_.y + confirmSize_.y*0.5f});
	memberSelectionMarker_->Update();

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
		teamMemberIcons_[i]->Initialize(sizukuIconHandle);
		teamMemberIcons_[i]->SetScale(iconSize_);
		teamMemberIcons_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

		inGameMemberIcons_[i] = std::make_unique<Sprite>();
		inGameMemberIcons_[i]->Initialize(sizukuIconHandle);
		inGameMemberIcons_[i]->SetScale({56.0f, 56.0f});
		inGameMemberIcons_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

		inGameMemberNameTexts_[i].Initialize(hudFontHandle_);
		inGameMemberNameTexts_[i].SetSize({180.0f, 40.0f});
		inGameMemberNameTexts_[i].SetAlign(TextAlign::Left);
		inGameMemberNameTexts_[i].SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		inGameMemberNameTexts_[i].SetString(U"");
		inGameMemberNameTexts_[i].UpdateLayout(false);

		slotPositions_[i] = {40.0f + (slotSize_.x + 18.0f) * i, 40.0f};
		teamSlotSprites_[i]->SetPosition(slotPositions_[i]);
		teamSlotSprites_[i]->Update();
		teamMemberCharacterIndices_[i] = -1;
	}
	// 初期チーム編成(操作キャラ+控えメンバー)
	if (!ownedCharacterIconHandles_.empty()) {
		occupiedSlots_[0] = true;
		teamMemberCharacterIndices_[0] = 0;
		teamMemberIcons_[0]->Initialize(ownedCharacterIconHandles_[0]);
	}
	if (ownedCharacterIconHandles_.size() >= 2) {
		occupiedSlots_[1] = true;
		teamMemberCharacterIndices_[1] = 1;
		teamMemberIcons_[1]->Initialize(ownedCharacterIconHandles_[1]);
	}
	activeSlotIndex_ = 0;
}

void Team::Update(bool isPartyOpen) {
	camera_->Update();
	teamBackgroundPlane_->SetCamera(camera_.get());
	teamBackgroundPlane_->Update();
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (Input::GetInstance()->TriggerKey(static_cast<BYTE>(DIK_1 + i)) && occupiedSlots_[i]) {
			if (activeSlotIndex_ != i) {
				characterSwitchTriggered_ = true;
			}
			activeSlotIndex_ = i;
		}
	}
	if (!isPartyOpen) {
		isDraggingInventoryIcon_ = false;
		draggingInventoryIndex_ = -1;
		dropTargetSlotIndex_ = -1;
		return;
	}
	Input::GetInstance()->SetIsCursorVisible(true);
	Input::GetInstance()->SetIsCursorStability(false);
	UpdatePartyUI();
}

void Team::UpdatePartyUI() {
	Vector2 mousePos{Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()};
	hoveredInventoryIndex_ = -1;
	dropTargetSlotIndex_ = -1;
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (IsInsideRect(mousePos, slotPositions_[i], slotSize_)) {
			dropTargetSlotIndex_ = i;
			if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft)) {
				activeSlotIndex_ = i;
			}
		}
	}

	for (size_t i = 0; i < inventoryIcons_.size(); ++i) {
		if (IsInsideRect(mousePos, inventoryIconPositions_[i], iconSize_)) {
			hoveredInventoryIndex_ = static_cast<int>(i);
			break;
		}
	}

	if (hoveredInventoryIndex_ >= 0 && Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft)) {
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
			draggingIcon_->SetPosition({mousePos.x - iconSize_.x * 0.5f, mousePos.y - iconSize_.y * 0.5f});
			draggingIcon_->SetColor({1.0f, 1.0f, 1.0f, dropTargetSlotIndex_ >= 0 ? 0.95f : 0.78f});
			draggingIcon_->Update();
		}
		if (Input::GetInstance()->ReleaseMouseButton(Input::MouseButton::kLeft)) {
			if (dropTargetSlotIndex_ >= 0) {
				AssignCharacterToSlot(dropTargetSlotIndex_, draggingInventoryIndex_);
			}
			isDraggingInventoryIcon_ = false;
			draggingInventoryIndex_ = -1;
		}
	}

	const bool isHoveringConfirm = IsInsideRect(mousePos, confirmPos_, confirmSize_);
	const bool isConfirmTriggered = isHoveringConfirm && (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerKey(DIK_SPACE));
	if (isConfirmTriggered) {
		AssignCharacterToSlot(activeSlotIndex_, selectedInventoryIndex_);
	}


	confirmButton_->SetColor(isHoveringConfirm ? Vector4{0.95f, 0.8f, 0.3f, 1.0f} : Vector4{0.2f, 0.55f, 0.9f, 0.95f});

	for (int i = 0; i < kMaxMembersCount; ++i) {
		Vector4 slotColor = {0.15f, 0.15f, 0.2f, 0.9f};
		if (isDraggingInventoryIcon_ && dropTargetSlotIndex_ == i) {
			slotColor = {0.95f, 0.8f, 0.3f, 1.0f};
		} else if (activeSlotIndex_ == i && occupiedSlots_[i]) {
			slotColor = {0.2f, 0.85f, 0.35f, 1.0f};
		}
		teamSlotSprites_[i]->SetColor(slotColor);
		teamSlotSprites_[i]->Update();
		if (occupiedSlots_[i]) {
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
}

void Team::Draw() {
	Object3dCommon::GetInstance()->DrawCommon();
	teamBackgroundPlane_->Draw();

	if (!isMemberSelectionActive_) {
		return;
	}
	SpriteCommon::GetInstance()->DrawCommon();
	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamSlotSprites_[i]->Draw();
		if (occupiedSlots_[i]) {
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
	confirmButton_->Draw();
	confirmButtonText_.Draw();
	if (isDraggingInventoryIcon_) {
		draggingIcon_->Draw();
	}
	if (isMemberSelectionActive_) {
		memberSelectionMarker_->Draw();
	}
}

void Team::DrawInGameMemberList() {
	int occupiedMemberCount = 0;
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (occupiedSlots_[i]) {
			++occupiedMemberCount;
		}
	}
	if (occupiedMemberCount < 2) {
		return;
	}
	int otherMemberCount = 0;
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (!occupiedSlots_[i] || i == activeSlotIndex_) {
			continue;
		}

		const int characterIndex = teamMemberCharacterIndices_[i];
		if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacterIconHandles_.size())) {
			continue;
		}

		const float baseX = 1224.0f;
		const float baseY = 250.0f;
		const float spacingY = 76.0f;
		const float y = baseY + spacingY * static_cast<float>(otherMemberCount);

		inGameMemberIcons_[i]->Initialize(ownedCharacterIconHandles_[characterIndex]);
		inGameMemberIcons_[i]->SetPosition({baseX, y});
		inGameMemberIcons_[i]->Update();
		inGameMemberIcons_[i]->Draw();

		inGameMemberNameTexts_[i].SetString(GetCharacterNameByIndex(characterIndex));
		inGameMemberNameTexts_[i].SetPosition({baseX + 62.0f, y + 8.0f});
		inGameMemberNameTexts_[i].UpdateLayout(false);
		inGameMemberNameTexts_[i].Update(false);
		inGameMemberNameTexts_[i].Draw();

		++otherMemberCount;
	}
}

bool Team::GetHasMember(int slotIndex) const {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return false;
	}
	return occupiedSlots_[slotIndex];
}

void Team::AssignCharacterToSlot(int slotIndex, int characterIndex) {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return;
	}
	if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacterIconHandles_.size())) {
		return;
	}
	int previousActiveCharacterIndex = -1;
	if (activeSlotIndex_ >= 0 && activeSlotIndex_ < kMaxMembersCount) {
		previousActiveCharacterIndex = teamMemberCharacterIndices_[activeSlotIndex_];
	}

	occupiedSlots_[slotIndex] = true;
	activeSlotIndex_ = slotIndex;
	selectedInventoryIndex_ = characterIndex;
	teamMemberCharacterIndices_[slotIndex] = characterIndex;
	teamMemberIcons_[slotIndex]->Initialize(ownedCharacterIconHandles_[characterIndex]);

	if (previousActiveCharacterIndex != characterIndex) {
		characterSwitchTriggered_ = true;
	}
}

bool Team::IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const { return point.x >= pos.x && point.x <= pos.x + size.x && point.y >= pos.y && point.y <= pos.y + size.y; }

const std::u32string& Team::GetCharacterNameByIndex(int characterIndex) const {
	static const std::u32string kEmptyName = U"";
	static const std::array<std::u32string, 4> kCharacterNames = {U"Sizuku", U"Mei", U"Arte", U"Yuzuki"};
	if (characterIndex < 0 || characterIndex >= static_cast<int>(kCharacterNames.size())) {
		return kEmptyName;
	}
	return kCharacterNames[characterIndex];
}
bool Team::ConsumeCharacterSwitchTriggered() {
	const bool triggered = characterSwitchTriggered_;
	characterSwitchTriggered_ = false;
	return triggered;
}
std::string Team::GetActiveCharacterName() const {
	if (activeSlotIndex_ < 0 || activeSlotIndex_ >= kMaxMembersCount) {
		return "Sizuku";
	}
	const int characterIndex = teamMemberCharacterIndices_[activeSlotIndex_];
	static const std::array<std::string, 4> kCharacterNames = {"Sizuku", "Mei", "Arte", "Yuzuki"};
	if (characterIndex < 0 || characterIndex >= static_cast<int>(kCharacterNames.size())) {
		return "Sizuku";
	}
	return kCharacterNames[characterIndex];
}
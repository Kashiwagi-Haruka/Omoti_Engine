#include "Team.h"
#include "Input.h"
#include "TextureManager.h"

void Team::Initialize() {
	whiteTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	iconTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Sizuku.png");

	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamSlotSprites_[i] = std::make_unique<Sprite>();
		teamSlotSprites_[i]->Initialize(whiteTextureHandle_);
		teamSlotSprites_[i]->SetScale(slotSize_);
		teamSlotSprites_[i]->SetColor({0.15f, 0.15f, 0.2f, 0.9f});

		teamMemberIcons_[i] = std::make_unique<Sprite>();
		teamMemberIcons_[i]->Initialize(iconTextureHandle_);
		teamMemberIcons_[i]->SetScale(iconSize_);
		teamMemberIcons_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

		slotPositions_[i] = {40.0f + (slotSize_.x + 18.0f) * i, 40.0f};
		teamSlotSprites_[i]->SetPosition(slotPositions_[i]);
		teamSlotSprites_[i]->Update();
	}

	inventoryBg_ = std::make_unique<Sprite>();
	inventoryBg_->Initialize(whiteTextureHandle_);
	inventoryBg_->SetPosition(inventoryPanelPos_);
	inventoryBg_->SetScale(inventoryPanelSize_);
	inventoryBg_->SetColor({0.08f, 0.08f, 0.1f, 0.92f});
	inventoryBg_->Update();

	inventoryIcon_ = std::make_unique<Sprite>();
	inventoryIcon_->Initialize(iconTextureHandle_);
	inventoryIcon_->SetScale(iconSize_);
	inventoryIconPositions_[0] = {120.0f, 240.0f};
	inventoryIcon_->SetPosition(inventoryIconPositions_[0]);
	inventoryIcon_->Update();

	confirmButton_ = std::make_unique<Sprite>();
	confirmButton_->Initialize(whiteTextureHandle_);
	confirmButton_->SetPosition(confirmPos_);
	confirmButton_->SetScale(confirmSize_);
	confirmButton_->SetColor({0.2f, 0.55f, 0.9f, 0.95f});
	confirmButton_->Update();

	candidatePreview_ = std::make_unique<Sprite>();
	candidatePreview_->Initialize(iconTextureHandle_);
	candidatePreview_->SetPosition(previewPos_);
	candidatePreview_->SetScale(previewSize_);
	candidatePreview_->Update();

	occupiedSlots_.fill(false);
	selectedSlotIndex_ = -1;
	isCandidateSelected_ = false;
	activeSlotIndex_ = 0;
}

void Team::Update(bool isPartyOpen) {
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (Input::GetInstance()->TriggerKey(static_cast<BYTE>(DIK_1 + i)) && occupiedSlots_[i]) {
			activeSlotIndex_ = i;
		}
	}

	if (!isPartyOpen) {
		return;
	}
	UpdatePartyUI();
}

void Team::UpdatePartyUI() {
	Vector2 mousePos{Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()};

	if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft)) {
		for (int i = 0; i < kMaxMembersCount; ++i) {
			if (IsInsideRect(mousePos, slotPositions_[i], slotSize_)) {
				selectedSlotIndex_ = i;
			}
		}

		if (IsInsideRect(mousePos, inventoryIconPositions_[0], iconSize_)) {
			isCandidateSelected_ = true;
		}

		if (selectedSlotIndex_ >= 0 && isCandidateSelected_ && IsInsideRect(mousePos, confirmPos_, confirmSize_)) {
			occupiedSlots_[selectedSlotIndex_] = true;
			activeSlotIndex_ = selectedSlotIndex_;
		}
	}

	for (int i = 0; i < kMaxMembersCount; ++i) {
		Vector4 slotColor = {0.15f, 0.15f, 0.2f, 0.9f};
		if (selectedSlotIndex_ == i) {
			slotColor = {0.9f, 0.8f, 0.2f, 1.0f};
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

	inventoryBg_->Update();
	inventoryIcon_->SetColor(isCandidateSelected_ ? Vector4{1.0f, 1.0f, 1.0f, 1.0f} : Vector4{0.8f, 0.8f, 0.8f, 1.0f});
	inventoryIcon_->Update();
	confirmButton_->SetColor((selectedSlotIndex_ >= 0 && isCandidateSelected_) ? Vector4{0.2f, 0.8f, 0.3f, 0.95f} : Vector4{0.2f, 0.55f, 0.9f, 0.95f});
	confirmButton_->Update();
	candidatePreview_->Update();
}

void Team::Draw() {
	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamSlotSprites_[i]->Draw();
		if (occupiedSlots_[i]) {
			teamMemberIcons_[i]->Draw();
		}
	}

	inventoryBg_->Draw();
	inventoryIcon_->Draw();
	if (isCandidateSelected_) {
		candidatePreview_->Draw();
	}
	confirmButton_->Draw();
}

bool Team::GetHasMember(int slotIndex) const {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return false;
	}
	return occupiedSlots_[slotIndex];
}

bool Team::IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const { return point.x >= pos.x && point.x <= pos.x + size.x && point.y >= pos.y && point.y <= pos.y + size.y; }
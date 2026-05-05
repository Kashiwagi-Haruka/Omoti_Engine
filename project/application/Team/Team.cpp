#include "Team.h"
#include "Input.h"
#include "Object/Character/Mei/Mei.h"
#include "Object/Character/Sizuku/Sizuku.h"
#include "TextureManager.h"

void Team::Initialize() {
	whiteTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	const uint32_t sizukuIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Sizuku.png");
	const uint32_t meiIconHandle = sizukuIconHandle;

	ownedCharacters_.clear();
	ownedCharacterIconHandles_.clear();
	ownedCharacters_.push_back(std::make_unique<Sizuku>());
	ownedCharacterIconHandles_.push_back(sizukuIconHandle);
	ownedCharacters_.push_back(std::make_unique<Mei>());
	ownedCharacterIconHandles_.push_back(meiIconHandle);

	for (int i = 0; i < kMaxMembersCount; ++i) {
		teamSlotSprites_[i] = std::make_unique<Sprite>();
		teamSlotSprites_[i]->Initialize(whiteTextureHandle_);
		teamSlotSprites_[i]->SetScale(slotSize_);
		teamSlotSprites_[i]->SetColor({0.15f, 0.15f, 0.2f, 0.9f});

		teamMemberIcons_[i] = std::make_unique<Sprite>();
		teamMemberIcons_[i]->Initialize(sizukuIconHandle);
		teamMemberIcons_[i]->SetScale(iconSize_);
		teamMemberIcons_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

		slotPositions_[i] = {40.0f + (slotSize_.x + 18.0f) * i, 40.0f};
		teamSlotSprites_[i]->SetPosition(slotPositions_[i]);
		teamSlotSprites_[i]->Update();
		teamMemberCharacterIndices_[i] = -1;
	}

	inventoryBg_ = std::make_unique<Sprite>();
	inventoryBg_->Initialize(whiteTextureHandle_);
	inventoryBg_->SetPosition(inventoryPanelPos_);
	inventoryBg_->SetScale(inventoryPanelSize_);
	inventoryBg_->SetColor({0.08f, 0.08f, 0.1f, 0.92f});
	inventoryBg_->Update();

	inventoryIcons_.clear();
	for (size_t i = 0; i < ownedCharacterIconHandles_.size(); ++i) {
		inventoryIconPositions_[i] = {120.0f + 100.0f * static_cast<float>(i), 240.0f};
		auto icon = std::make_unique<Sprite>();
		icon->Initialize(ownedCharacterIconHandles_[i]);
		icon->SetScale(iconSize_);
		icon->SetPosition(inventoryIconPositions_[i]);
		icon->Update();
		inventoryIcons_.push_back(std::move(icon));
	}

	confirmButton_ = std::make_unique<Sprite>();
	confirmButton_->Initialize(whiteTextureHandle_);
	confirmButton_->SetPosition(confirmPos_);
	confirmButton_->SetScale(confirmSize_);
	confirmButton_->SetColor({0.2f, 0.55f, 0.9f, 0.95f});
	confirmButton_->Update();

	candidatePreview_ = std::make_unique<Sprite>();
	candidatePreview_->Initialize(sizukuIconHandle);
	candidatePreview_->SetPosition(previewPos_);
	candidatePreview_->SetScale(previewSize_);
	candidatePreview_->Update();

	occupiedSlots_.fill(false);
	selectedSlotIndex_ = 0;
	isCandidateSelected_ = true;
	activeSlotIndex_ = 0;

	// 初期チーム: Sizuku, Mei
	for (int i = 0; i < 2; ++i) {
		occupiedSlots_[i] = true;
		teamMemberCharacterIndices_[i] = i;
		teamMemberIcons_[i]->Initialize(ownedCharacterIconHandles_[i]);
	}
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

void Team::UpdatePartyUI() { /* existing interactive editing omitted */
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
	for (auto& icon : inventoryIcons_) {
		icon->SetColor({1, 1, 1, 1});
		icon->Update();
	}
	inventoryBg_->Update();
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
	for (auto& icon : inventoryIcons_) {
		icon->Draw();
	}
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
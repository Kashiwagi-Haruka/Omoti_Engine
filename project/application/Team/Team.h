#pragma once
#include "Sprite.h"
#include "Vector2.h"
#include <array>
#include <memory>

class Team {
public:
	void Initialize();
	void Update(bool isPartyOpen);
	void Draw();

	bool GetHasMember(int slotIndex) const;
	int GetActiveSlot() const { return activeSlotIndex_; }

private:
	static constexpr int kMaxMembersCount = 4;

	void UpdatePartyUI();
	bool IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const;

	uint32_t whiteTextureHandle_ = 0;
	uint32_t iconTextureHandle_ = 0;

	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamSlotSprites_{};
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamMemberIcons_{};
	std::array<bool, kMaxMembersCount> occupiedSlots_{};
	std::array<Vector2, kMaxMembersCount> slotPositions_{};
	std::array<Vector2, kMaxMembersCount> inventoryIconPositions_{};
	std::unique_ptr<Sprite> inventoryBg_;
	std::unique_ptr<Sprite> inventoryIcon_;
	std::unique_ptr<Sprite> confirmButton_;
	std::unique_ptr<Sprite> candidatePreview_;

	Vector2 slotSize_{80.0f, 80.0f};
	Vector2 inventoryPanelPos_{70.0f, 170.0f};
	Vector2 inventoryPanelSize_{360.0f, 420.0f};
	Vector2 iconSize_{64.0f, 64.0f};
	Vector2 confirmPos_{930.0f, 580.0f};
	Vector2 confirmSize_{220.0f, 70.0f};
	Vector2 previewPos_{900.0f, 380.0f};
	Vector2 previewSize_{128.0f, 128.0f};

	int selectedSlotIndex_ = -1;
	bool isCandidateSelected_ = false;
	int activeSlotIndex_ = 0;
};
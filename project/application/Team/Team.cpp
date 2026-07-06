#include "Team.h"
#include "Input.h"
#include "Object/Characters/Playable/Individual/Arte/Arte.h"
#include "Object/Characters/Playable/Individual/Mei/Mei.h"
#include "Object/Characters/Playable/Individual/Sizuku/Sizuku.h"
#include "Object/Characters/Playable/Individual/Yuzuki/Yuzuki.h"
#include "TextureManager.h"
#include "StringUtility.h"

namespace {
std::u32string ToU32String(const std::string& utf8String) {
	const std::wstring wideString = StringUtility::ConvertString_(utf8String);
	return std::u32string(wideString.begin(), wideString.end());
}
} // namespace

void Team::Initialize() {
	const uint32_t sizukuIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Sizuku.png");
	const uint32_t meiIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Mei.png");
	const uint32_t arteIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Arte.png");
	const uint32_t yuzukiIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Character/Icon/Yuzuki.png");

	ownedCharacters_.clear();
	ownedCharacterIconHandles_.clear();
	ownedCharacterDisplayNames_.clear();
	ownedCharacters_.push_back(std::make_unique<Sizuku>());
	ownedCharacterIconHandles_.push_back(sizukuIconHandle);
	ownedCharacters_.push_back(std::make_unique<Mei>());
	ownedCharacterIconHandles_.push_back(meiIconHandle);
	ownedCharacters_.push_back(std::make_unique<Arte>());
	ownedCharacterIconHandles_.push_back(arteIconHandle);
	ownedCharacters_.push_back(std::make_unique<Yuzuki>());
	ownedCharacterIconHandles_.push_back(yuzukiIconHandle);

	ownedCharacterDisplayNames_.reserve(ownedCharacters_.size());
	for (auto& character : ownedCharacters_) {
		character->Initialize();
		ownedCharacterDisplayNames_.push_back(ToU32String(character->GetName()));
	}

	teamMemberCharacterIndices_.fill(-1);
	occupiedSlots_.fill(false);
	if (!ownedCharacterIconHandles_.empty()) {
		occupiedSlots_[0] = true;
		teamMemberCharacterIndices_[0] = 0;
	}
	if (ownedCharacterIconHandles_.size() >= 2) {
		occupiedSlots_[1] = true;
		teamMemberCharacterIndices_[1] = 1;
	}
	activeSlotIndex_ = 0;
	characterSwitchTriggered_ = false;
}

void Team::Update() {
	for (int i = 0; i < kMaxMembersCount; ++i) {
		if (Input::GetInstance()->TriggerKey(static_cast<BYTE>(DIK_1 + i)) && occupiedSlots_[i]) {
			SetActiveSlot(i);
		}
	}
}

bool Team::GetHasMember(int slotIndex) const {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return false;
	}
	return occupiedSlots_[slotIndex];
}

int Team::GetMemberCharacterIndex(int slotIndex) const {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return -1;
	}
	return teamMemberCharacterIndices_[slotIndex];
}

uint32_t Team::GetOwnedCharacterIconHandle(int characterIndex) const {
	if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacterIconHandles_.size())) {
		return 0;
	}
	return ownedCharacterIconHandles_[characterIndex];
}

void Team::AssignCharacterToSlot(int slotIndex, int characterIndex) {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount) {
		return;
	}
	if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacterIconHandles_.size())) {
		return;
	}
	const int previousActiveCharacterIndex = GetMemberCharacterIndex(activeSlotIndex_);
	occupiedSlots_[slotIndex] = true;
	activeSlotIndex_ = slotIndex;
	teamMemberCharacterIndices_[slotIndex] = characterIndex;
	if (previousActiveCharacterIndex != characterIndex) {
		characterSwitchTriggered_ = true;
	}
}

void Team::SetActiveSlot(int slotIndex) {
	if (slotIndex < 0 || slotIndex >= kMaxMembersCount || !occupiedSlots_[slotIndex]) {
		return;
	}
	if (activeSlotIndex_ != slotIndex) {
		characterSwitchTriggered_ = true;
	}
	activeSlotIndex_ = slotIndex;
}

bool Team::ConsumeCharacterSwitchTriggered() {
	const bool triggered = characterSwitchTriggered_;
	characterSwitchTriggered_ = false;
	return triggered;
}

const std::u32string& Team::GetCharacterNameByIndex(int characterIndex) const {
	static const std::u32string kEmptyName = U"";
	if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacterDisplayNames_.size())) {
		return kEmptyName;
	}
	return ownedCharacterDisplayNames_[characterIndex];
}

std::string Team::GetPlayableNameByIndex(int characterIndex) const {
	if (characterIndex < 0 || characterIndex >= static_cast<int>(ownedCharacters_.size())) {
		return "Sizuku";
	}
	if (!ownedCharacters_[characterIndex]) {
		return "Sizuku";
	}
	const std::string& playableName = ownedCharacters_[characterIndex]->GetRomanizationName();
	return playableName.empty() ? "Sizuku" : playableName;
}

std::string Team::GetActiveCharacterName() const {
	if (activeSlotIndex_ < 0 || activeSlotIndex_ >= kMaxMembersCount) {
		return "Sizuku";
	}
	if (!occupiedSlots_[activeSlotIndex_]) {
		return "Sizuku";
	}
	return GetPlayableNameByIndex(teamMemberCharacterIndices_[activeSlotIndex_]);
}
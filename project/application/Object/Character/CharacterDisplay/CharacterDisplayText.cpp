#include "CharacterDisplayText.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"
CharacterDisplayText::CharacterDisplayText() = default;

void CharacterDisplayText::Initialize() {
	uint32_t handle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(handle, 72, 72);
	CharacterNameText_.Initialize(handle);
	CharacterLevelText_.Initialize(handle);
}
void CharacterDisplayText::Update() {
	CharacterNameText_.Update();
	CharacterLevelText_.Update();
}
void CharacterDisplayText::Draw() {
	CharacterNameText_.Draw();
	CharacterLevelText_.Draw();
}
void CharacterDisplayText::SetCharacterName(const std::u32string& name) {
	CharacterNameText_.SetString(name);
}
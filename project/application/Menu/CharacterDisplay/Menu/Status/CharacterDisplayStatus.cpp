#include "CharacterDisplayStatus.h"
#include "SpriteCommon.h"
#include "Text/FreeTypeManager/FreeTypeManager.h"

namespace {
constexpr float kTextPositionX = 850.0f;
constexpr float kTextPositionY = 220.0f;
constexpr float kTextMarginY = 60.0f;
constexpr Vector2 kTextSize = {360.0f, 60.0f};
constexpr Vector4 kTextColor = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace

void CharacterDisplayStatus::Initialize() {
	const uint32_t fontHandle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);
	FreeTypeManager::SetPixelSizes(fontHandle, 28, 28);

	auto initializeText = [fontHandle](Text& text, const std::u32string& string, float positionY) {
		text.Initialize(fontHandle);
		text.SetSize(kTextSize);
		text.SetPosition({kTextPositionX, positionY});
		text.SetAlign(TextAlign::Left);
		text.SetColor(kTextColor);
		text.SetString(string);
		text.UpdateLayout(false);
	};

	initializeText(nameText_, U"名前", kTextPositionY);
	initializeText(levelText_, U"レベル", kTextPositionY + kTextMarginY);
	initializeText(expText_, U"経験値", kTextPositionY + kTextMarginY * 2.0f);
	initializeText(statusText_, U"ステータス", kTextPositionY + kTextMarginY * 3.0f);
}

void CharacterDisplayStatus::Update() {
	nameText_.Update(false);
	levelText_.Update(false);
	expText_.Update(false);
	statusText_.Update(false);
}

void CharacterDisplayStatus::Draw() {
	SpriteCommon::GetInstance()->DrawCommonFont();
	nameText_.Draw();
	levelText_.Draw();
	expText_.Draw();
	statusText_.Draw();
}
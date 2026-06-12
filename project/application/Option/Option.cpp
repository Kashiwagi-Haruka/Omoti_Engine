#include "Option.h"
#include "Text/FreetypeManager/FreeTypeManager.h"
namespace{
static const int kMenuCount = 3;
std::u32string menuStrings[kMenuCount] = {U"カメラ", U"サウンド", U"戻る"};
}

Option::Option() {}
Option::~Option() {}

void Option::Initialize() {
	menuTexts_.clear();
	uint32_t fontHandle = FreeTypeManager::CreateFace("Resources/Font/JF-Dot-jiskan24-2000.ttf", 0);
	for (int i = 0; i < kMenuCount; ++i) {
		Text text;
		text.Initialize(fontHandle);
		text.SetSize({1280.0f, 100.0f});
		text.SetString(menuStrings[i]);
		text.SetPosition({640.0f, 200.0f + i * 100.0f});
		text.SetAlign(TextAlign::Center);
		text.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		text.UpdateLayout(false);
		menuTexts_.push_back(text);
	}
}
void Option::Update() {}
void Option::Draw() {
	for (auto& menuText : menuTexts_) {
		menuText.Draw();
	}
}
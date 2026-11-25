#define NOMINMAX
#include "TutorialProgressWindow.h"

#include "TextureManager.h"

#include <algorithm>
#include <array>
#include <cstddef>

namespace {

using Step = TutorialFlow::Step;

// TutorialFlow::Step の列挙順に対応する。
constexpr std::array<const char32_t*, 8> kStepNames{
    U"移動", U"視点操作", U"通常攻撃", U"スキル", U"必殺技", U"属性変更", U"ポーズ", U"完了",
};

void AppendRow(std::u32string& text, Step step, int percent) {
	text += U"\n・";
	text += kStepNames[static_cast<std::size_t>(step)];
	text += U' ';

	const std::string digits = std::to_string(percent);
	text.append(digits.begin(), digits.end());
	text += U'%';
}

} // namespace

void TutorialProgressWindow::Initialize(std::uint32_t fontHandle) {
	const auto textureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/tuatorial/TutorialWindow.png");

	background_.Initialize(textureHandle);
	background_.SetAnchorPoint({0.0f, 0.0f});
	background_.SetPosition({100.0f, 150.0f});
	background_.SetScale({360.0f, 110.0f});
	background_.Update();

	text_.Initialize(fontHandle);
	text_.SetPosition({118.0f, 168.0f});
	text_.SetSize({324.0f, 92.0f});
	text_.SetAlign(TextAlign::Left);
	text_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	displayedText_.clear();
}

void TutorialProgressWindow::Update(TutorialFlow::Step currentStep, float stepProgress) {
	const int percent = static_cast<int>(std::clamp(stepProgress, 0.0f, 1.0f) * 100.0f);

	std::u32string nextText = U"[進捗]";

	switch (currentStep) {
	case Step::Move:
		AppendRow(nextText, Step::Move, percent);
		AppendRow(nextText, Step::Look, 0);
		break;

	case Step::Look:
		AppendRow(nextText, Step::Move, 100);
		AppendRow(nextText, Step::Look, percent);
		break;

	default:
		AppendRow(nextText, currentStep, percent);
		break;
	}

	if (nextText == displayedText_) {
		return;
	}

	displayedText_ = nextText;
	text_.SetString(displayedText_);
	text_.UpdateLayout(false);
}

void TutorialProgressWindow::DrawBackground() { background_.Draw(); }

void TutorialProgressWindow::DrawText() { text_.Draw(); }
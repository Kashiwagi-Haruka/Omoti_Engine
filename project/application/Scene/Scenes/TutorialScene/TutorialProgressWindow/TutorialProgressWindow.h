#pragma once

#include "Sprite.h"
#include "Text/Text.h"
#include "Scene/Scenes/TutorialScene/TutorialFlow/TutorialFlow.h"

#include <cstdint>
#include <string>

class TutorialProgressWindow {
public:
	void Initialize(std::uint32_t fontHandle);
	void Update(TutorialFlow::Step currentStep, float stepProgress);
	void DrawBackground();
	void DrawText();

private:
	Sprite background_;
	Text text_;
	std::u32string displayedText_;
};
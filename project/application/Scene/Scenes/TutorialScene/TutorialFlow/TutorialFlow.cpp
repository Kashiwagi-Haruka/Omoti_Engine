#include "TutorialFlow.h"
#include "Scene/Scenes/TutorialScene/TutorialStep/TutorialState.h"

#include <algorithm>

namespace {

std::u32string ToText(std::size_t value) {
	const std::string text = std::to_string(value);
	return std::u32string(text.begin(), text.end());
}

} // namespace

TutorialFlow::TutorialFlow() {
	steps_.push_back(MakeMoveStep());
	steps_.push_back(MakeLookStep());
	steps_.push_back(MakeAttackStep());
	steps_.push_back(MakeSkillStep());
	steps_.push_back(MakeSpecialStep());
	steps_.push_back(MakeAttributeStep());
	steps_.push_back(MakePauseStep());
	steps_.push_back(MakeCompleteStep());
}

TutorialFlow::~TutorialFlow() = default;

void TutorialFlow::Reset(const TutorialFrame& frame) {
	index_ = 0;
	stepProgress_ = 0.0f;
	finished_ = false;
	steps_.front()->Enter(frame);
}

void TutorialFlow::Update(const TutorialFrame& frame) {
	if (finished_) {
		return;
	}

	auto& current = *steps_[index_];

	// ポーズの課題は、停止中のフレームで開いたことを記録する。
	if (frame.isPaused && current.Id() != Step::Pause) {
		return;
	}

	const auto result = current.Update(frame);
	stepProgress_ = std::clamp(result.progress, 0.0f, 1.0f);

	if (!result.done) {
		return;
	}

	if (index_ + 1 == steps_.size()) {
		stepProgress_ = 1.0f;
		finished_ = true;
		return;
	}

	++index_;
	stepProgress_ = 0.0f;
	steps_[index_]->Enter(frame);
}

TutorialFlow::Step TutorialFlow::CurrentStep() const { return steps_[index_]->Id(); }

bool TutorialFlow::IsFinished() const { return finished_; }

float TutorialFlow::Progress() const { return (static_cast<float>(index_) + stepProgress_) / static_cast<float>(steps_.size()); }

std::u32string TutorialFlow::Guide(const TutorialFrame& frame) const { return U"チュートリアル " + ToText(index_ + 1) + U" / " + ToText(steps_.size()) + U"\n" + steps_[index_]->Guide(frame); }
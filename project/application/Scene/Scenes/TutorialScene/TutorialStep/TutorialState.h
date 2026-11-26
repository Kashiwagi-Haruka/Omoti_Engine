#pragma once

#include "Scene/Scenes/TutorialScene/TutorialFlow/TutorialFlow.h"

class TutorialState {
public:
	struct Result {
		float progress;
		bool done;
	};

	explicit TutorialState(TutorialFlow::Step step);
	virtual ~TutorialState() = default;

	TutorialFlow::Step Id() const;
	virtual void Enter(const TutorialFrame& frame);
	virtual Result Update(const TutorialFrame& frame) = 0;
	virtual std::u32string Guide(const TutorialFrame& frame) const = 0;

private:
	TutorialFlow::Step step_;
};

std::unique_ptr<TutorialState> MakeMoveStep();
std::unique_ptr<TutorialState> MakeLookStep();
std::unique_ptr<TutorialState> MakeAttackStep();
std::unique_ptr<TutorialState> MakeSkillStep();
std::unique_ptr<TutorialState> MakeSpecialStep();
std::unique_ptr<TutorialState> MakeAttributeStep();
std::unique_ptr<TutorialState> MakePauseStep();
std::unique_ptr<TutorialState> MakeCompleteStep();
std::unique_ptr<TutorialState> MakeAttributeAttackStep();
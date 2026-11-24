#pragma once

#include "Object/Characters/Base/Attribute.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct TutorialFrame {
	float deltaTime = 0.0f;
	bool isPaused = false;
	bool moving = false;
	bool looking = false;
	bool attacking = false;
	bool skillStarted = false;
	bool specialStarted = false;
	bool specialOnCooldown = false;
	Attribute attribute = Attribute::None;
};

class TutorialState;

class TutorialFlow {
public:
	enum class Step {
		Move,
		Look,
		Attack,
		Skill,
		Special,
		AttributeChange,
		Pause,
		Complete,
	};

	TutorialFlow();
	~TutorialFlow();

	void Reset(const TutorialFrame& frame);
	void Update(const TutorialFrame& frame);

	Step CurrentStep() const;
	bool IsFinished() const;
	float Progress() const;
	std::u32string Guide(const TutorialFrame& frame) const;

private:
	std::vector<std::unique_ptr<TutorialState>> steps_;
	std::size_t index_ = 0;
	float stepProgress_ = 0.0f;
	bool finished_ = false;
};
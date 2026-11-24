#include "TutorialState.h"

TutorialState::TutorialState(TutorialFlow::Step step) : step_(step) {}

TutorialFlow::Step TutorialState::Id() const { return step_; }

void TutorialState::Enter(const TutorialFrame&) {}

namespace {

using Step = TutorialFlow::Step;

TutorialState::Result FromDone(bool done) { return {done ? 1.0f : 0.0f, done}; }

std::u32string ToText(std::size_t value) {
	const std::string text = std::to_string(value);
	return std::u32string(text.begin(), text.end());
}

class TimedStep final : public TutorialState {
public:
	using Condition = bool TutorialFrame::*;

	TimedStep(Step step, float seconds, const char32_t* guide, Condition condition = nullptr) : TutorialState(step), seconds_(seconds), guide_(guide), condition_(condition) {}

	void Enter(const TutorialFrame&) override { elapsed_ = 0.0f; }

	Result Update(const TutorialFrame& frame) override {
		if (condition_ == nullptr || frame.*condition_) {
			elapsed_ += frame.deltaTime;
		}

		return {elapsed_ / seconds_, elapsed_ >= seconds_};
	}

	std::u32string Guide(const TutorialFrame&) const override { return guide_; }

private:
	float seconds_;
	std::u32string guide_;
	Condition condition_;
	float elapsed_ = 0.0f;
};

class AttackStep final : public TutorialState {
public:
	AttackStep() : TutorialState(Step::Attack) {}

	Result Update(const TutorialFrame& frame) override { return FromDone(frame.attacking); }

	std::u32string Guide(const TutorialFrame&) const override { return U"左クリック / Xボタンで攻撃しよう"; }
};

class SkillStep final : public TutorialState {
public:
	SkillStep() : TutorialState(Step::Skill) {}

	void Enter(const TutorialFrame&) override { count_ = 0; }

	Result Update(const TutorialFrame& frame) override {
		if (frame.skillStarted) {
			++count_;
		}

		return {
		    static_cast<float>(count_) / static_cast<float>(kTargetCount),
		    count_ >= kTargetCount,
		};
	}

	std::u32string Guide(const TutorialFrame&) const override { return U"E / RBでスキルを" + ToText(kTargetCount) + U"回使おう\n発動回数 " + ToText(count_) + U" / " + ToText(kTargetCount); }

private:
	static constexpr int kTargetCount = 3;
	int count_ = 0;
};

class SpecialStep final : public TutorialState {
public:
	SpecialStep() : TutorialState(Step::Special) {}

	Result Update(const TutorialFrame& frame) override { return FromDone(frame.specialStarted); }

	std::u32string Guide(const TutorialFrame& frame) const override {
		std::u32string text = U"Q / Yボタンで必殺技を使おう";

		if (frame.specialOnCooldown) {
			text += U"\nクールタイムが終わるまで待とう";
		}

		return text;
	}
};

class AttributeStep final : public TutorialState {
public:
	AttributeStep() : TutorialState(Step::AttributeChange) {}

	void Enter(const TutorialFrame& frame) override { initialAttribute_ = frame.attribute; }

	Result Update(const TutorialFrame& frame) override { return FromDone(frame.attribute != initialAttribute_); }

	std::u32string Guide(const TutorialFrame&) const override {
		return U"今と違う属性に変更しよう\n"
		       U"Yキーを押しながらマウスで選択\n"
		       U"パッド：LTを押しながら左スティック";
	}

private:
	Attribute initialAttribute_ = Attribute::None;
};

class PauseStep final : public TutorialState {
public:
	PauseStep() : TutorialState(Step::Pause) {}

	void Enter(const TutorialFrame&) override { opened_ = false; }

	Result Update(const TutorialFrame& frame) override {
		if (frame.isPaused) {
			opened_ = true;
			return {0.0f, false};
		}

		return FromDone(opened_);
	}

	std::u32string Guide(const TutorialFrame&) const override { return opened_ ? U"もう一度押してゲームを再開しよう" : U"Esc / STARTでポーズしてみよう"; }

private:
	bool opened_ = false;
};

} // namespace

std::unique_ptr<TutorialState> MakeMoveStep() { return std::make_unique<TimedStep>(Step::Move, 2.0f, U"WASD / 左スティックで移動しよう\n合計2秒間、移動してみよう", &TutorialFrame::moving); }

std::unique_ptr<TutorialState> MakeLookStep() {
	return std::make_unique<TimedStep>(
	    Step::Look, 2.0f,
	    U"マウス / 右スティックで視点を動かそう\n"
	    U"合計2秒間、視点を動かしてみよう",
	    &TutorialFrame::looking);
}

std::unique_ptr<TutorialState> MakeAttackStep() { return std::make_unique<AttackStep>(); }

std::unique_ptr<TutorialState> MakeSkillStep() { return std::make_unique<SkillStep>(); }

std::unique_ptr<TutorialState> MakeSpecialStep() { return std::make_unique<SpecialStep>(); }

std::unique_ptr<TutorialState> MakeAttributeStep() { return std::make_unique<AttributeStep>(); }

std::unique_ptr<TutorialState> MakePauseStep() { return std::make_unique<PauseStep>(); }

std::unique_ptr<TutorialState> MakeCompleteStep() { return std::make_unique<TimedStep>(Step::Complete, 2.0f, U"チュートリアル完了！\nまもなくゲームを開始します"); }
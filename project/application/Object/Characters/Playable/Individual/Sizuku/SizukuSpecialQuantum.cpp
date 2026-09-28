#define NOMINMAX
#include "SizukuSpecialQuantum.h"
#include "SizukuSpecial.h"

void SizukuSpecial::StartQuantumSpecial() {
	// 量子属性は左右へ位置をずらした二段階の空間破裂。
	animationTimeMax_ = 4.2f;
	particleTransform_ = sizukuTransform_;
	damagePosition_ = sizukuTransform_.translate;
	damageScale_ = {11.0f, 6.0f, 11.0f};
	ConfigureEmitter(*mainEmitter_, {0.55f, 0.12f, 1.0f, 1.0f}, 130, 9.0f, 1.5f);
	ConfigureEmitter(*subEmitter_, {0.15f, 0.65f, 1.0f, 1.0f}, 130, 9.0f, 1.5f);
}

void SizukuSpecial::UpdateQuantumSpecial(float) {
	if (!attributeEffectEmitted_ && elapsedTime_ >= 0.9f) {
		Transform left = particleTransform_;
		left.translate.x -= 5.0f;
		mainEmitter_->SetTransform(left);
		mainEmitter_->Emit();
		Transform right = particleTransform_;
		right.translate.x += 5.0f;
		subEmitter_->SetTransform(right);
		subEmitter_->Emit();
		attributeEffectEmitted_ = true;
	}
}

void SizukuSpecialQuantum::Start(SizukuSpecial& special) { special.StartQuantumSpecial(); }

void SizukuSpecialQuantum::Update(SizukuSpecial& special, float deltaTime) { special.UpdateQuantumSpecial(deltaTime); }
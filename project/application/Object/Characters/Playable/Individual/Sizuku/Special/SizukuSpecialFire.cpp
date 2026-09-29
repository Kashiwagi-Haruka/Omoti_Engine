#define NOMINMAX
#include "SizukuSpecialFire.h"
#include "SizukuSpecial.h"
#include "Function.h"

void SizukuSpecial::StartFireSpecial() {
	// 火属性は上空で炸裂する花火。氷花や氷雨は使用しない。
	animationTimeMax_ = 4.0f;
	particleTransform_ = sizukuTransform_;
	particleTransform_.translate.y += 8.0f;
	damagePosition_ = particleTransform_.translate;
	damageScale_ = {12.0f, 10.0f, 12.0f};
	ConfigureEmitter(*mainEmitter_, {1.0f, 0.18f, 0.04f, 1.0f}, 180, 12.0f, 1.8f);
	ConfigureEmitter(*subEmitter_, {1.0f, 0.85f, 0.15f, 1.0f}, 100, 8.0f, 1.4f);
}

void SizukuSpecial::UpdateFireSpecial(float) {
	particleTransform_.translate = sizukuTransform_.translate + Vector3{0.0f, 8.0f, 0.0f};
	mainEmitter_->SetTransform(particleTransform_);
	subEmitter_->SetTransform(particleTransform_);
	if (!fireworkEmitted_ && elapsedTime_ >= 1.0f) {
		mainEmitter_->Emit();
		subEmitter_->Emit();
		fireworkEmitted_ = true;
	}
}

void SizukuSpecialFire::Start(SizukuSpecial& special) { special.StartFireSpecial(); }

void SizukuSpecialFire::Update(SizukuSpecial& special, float deltaTime) { special.UpdateFireSpecial(deltaTime); }
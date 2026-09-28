#define NOMINMAX
#include "SizukuSpecialImaginary.h"
#include "SizukuSpecial.h"

void SizukuSpecial::StartImaginarySpecial() {
	// 虚数属性は頭上に光を集め、広範囲へ一度だけ解放する。
	animationTimeMax_ = 4.0f;
	particleTransform_ = sizukuTransform_;
	particleTransform_.translate.y += 5.0f;
	damagePosition_ = sizukuTransform_.translate;
	damageScale_ = {14.0f, 6.0f, 14.0f};
	ConfigureEmitter(*mainEmitter_, {1.0f, 0.78f, 0.18f, 1.0f}, 220, 4.0f, 2.2f);
}

void SizukuSpecial::UpdateImaginarySpecial(float) {
	mainEmitter_->SetTransform(particleTransform_);
	if (!attributeEffectEmitted_ && elapsedTime_ >= 1.5f) {
		mainEmitter_->Emit();
		attributeEffectEmitted_ = true;
	}
}

void SizukuSpecialImaginary::Start(SizukuSpecial& special) { special.StartImaginarySpecial(); }

void SizukuSpecialImaginary::Update(SizukuSpecial& special, float deltaTime) { special.UpdateImaginarySpecial(deltaTime); }
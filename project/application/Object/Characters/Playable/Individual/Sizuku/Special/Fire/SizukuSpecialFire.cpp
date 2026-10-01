#include "SizukuSpecialFire.h"
#include "SizukuSpecialParticle.h"
#include "Function.h"
void SizukuSpecialFire::Initialize() {
	mainEmitter_ = CreateSpecialEmitter("sizukuSpecialFireMain");
	subEmitter_ = CreateSpecialEmitter("sizukuSpecialFireSub");
}
void SizukuSpecialFire::Start(const SizukuSpecialContext& c) {
	emitted_ = false;
	particleTransform_ = c.transform;
	particleTransform_.translate.y += 8.0f;
	damagePosition_ = particleTransform_.translate;
	damageScale_ = {12, 10, 12};
	ConfigureSpecialEmitter(*mainEmitter_, {1, .18f, .04f, 1}, 180, 12, 1.8f);
	ConfigureSpecialEmitter(*subEmitter_, {1, .85f, .15f, 1}, 100, 8, 1.4f);
}
void SizukuSpecialFire::Update(const SizukuSpecialContext& c, float) {
	particleTransform_.translate = c.transform.translate + Vector3{0, 8, 0};
	mainEmitter_->SetTransform(particleTransform_);
	subEmitter_->SetTransform(particleTransform_);
	if (!emitted_ && c.elapsedTime >= 1) {
		mainEmitter_->Emit();
		subEmitter_->Emit();
		emitted_ = true;
	}
}
void SizukuSpecialFire::Draw() {
	mainEmitter_->Draw();
	subEmitter_->Draw();
}
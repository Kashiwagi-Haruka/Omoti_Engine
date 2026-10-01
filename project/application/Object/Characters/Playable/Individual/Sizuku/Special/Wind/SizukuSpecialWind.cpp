#include "SizukuSpecialWind.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialParticle.h"
void SizukuSpecialWind::Initialize() { emitter_ = CreateSpecialEmitter("sizukuSpecialWind"); }
void SizukuSpecialWind::Start(const SizukuSpecialContext& c) {
	particleTransform_ = c.transform;
	damagePosition_ = c.transform.translate;
	damageScale_ = {10, 8, 10};
	ConfigureSpecialEmitter(*emitter_, {.25f, 1, .55f, 1}, 45, 7, 1.2f);
	emitter_->SetAcceleration({0, 5, 0});
	emitter_->SetFrequency(.12f);
}
void SizukuSpecialWind::Update(const SizukuSpecialContext& c, float) {
	particleTransform_.translate = c.transform.translate;
	emitter_->Update(particleTransform_);
	damagePosition_ = c.transform.translate;
}
void SizukuSpecialWind::Draw() { emitter_->Draw(); }
#include "SizukuSpecialQuantum.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialParticle.h"
void SizukuSpecialQuantum::Initialize() {
	main_ = CreateSpecialEmitter("sizukuSpecialQuantumMain");
	sub_ = CreateSpecialEmitter("sizukuSpecialQuantumSub");
}
void SizukuSpecialQuantum::Start(const SizukuSpecialContext& c) {
	origin_ = c.transform;
	damagePosition_ = c.transform.translate;
	damageScale_ = {11, 6, 11};
	emitted_ = false;
	ConfigureSpecialEmitter(*main_, {.55f, .12f, 1, 1}, 130, 9, 1.5f);
	ConfigureSpecialEmitter(*sub_, {.15f, .65f, 1, 1}, 130, 9, 1.5f);
}
void SizukuSpecialQuantum::Update(const SizukuSpecialContext& c, float) {
	if (!emitted_ && c.elapsedTime >= .9f) {
		Transform l = origin_;
		l.translate.x -= 5;
		main_->SetTransform(l);
		main_->Emit();
		Transform r = origin_;
		r.translate.x += 5;
		sub_->SetTransform(r);
		sub_->Emit();
		emitted_ = true;
	}
}
void SizukuSpecialQuantum::Draw() {
	main_->Draw();
	sub_->Draw();
}
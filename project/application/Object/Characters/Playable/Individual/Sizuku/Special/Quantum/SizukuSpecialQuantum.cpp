#define NOMINMAX
#include "SizukuSpecialQuantum.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>

SizukuSpecialQuantum::~SizukuSpecialQuantum() { End(); }

void SizukuSpecialQuantum::End() {
	if (!postEffectsActive_) {
		return;
	}
	auto* common = Object3dCommon::GetInstance();
	common->SetFullScreenGrayscaleOverlayIntensity(0.0f);
	common->SetGlitchEnabled(previousGlitchEnabled_);
	common->SetGlitchIntensity(previousGlitchIntensity_);
	postEffectsActive_ = false;
}
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialParticle.h"
void SizukuSpecialQuantum::Initialize() {
	main_ = CreateSpecialEmitter("sizukuSpecialQuantumMain");
	sub_ = CreateSpecialEmitter("sizukuSpecialQuantumSub");
}
void SizukuSpecialQuantum::Start(const SizukuSpecialContext& c) {
	End();
	auto* common = Object3dCommon::GetInstance();
	previousGlitchEnabled_ = common->GetGlitchEnabled();
	previousGlitchIntensity_ = common->GetGlitchIntensity();
	postEffectsActive_ = true;
	common->SetGlitchEnabled(true);
	common->SetGlitchIntensity(0.0f);
	origin_ = c.transform;
	damagePosition_ = c.transform.translate;
	damageScale_ = {11, 6, 11};
	emitted_ = false;
	ConfigureSpecialEmitter(*main_, {.55f, .12f, 1, 1}, 130, 9, 1.5f);
	ConfigureSpecialEmitter(*sub_, {.15f, .65f, 1, 1}, 130, 9, 1.5f);
}
void SizukuSpecialQuantum::Update(const SizukuSpecialContext& c, float) {
	
	const float charge = std::clamp(c.elapsedTime / 0.9f, 0.0f, 1.0f);
	const float fade = 1.0f - std::clamp((c.elapsedTime - 0.9f) / 1.5f, 0.0f, 1.0f);
	const float intensity = charge * fade;
	auto* common = Object3dCommon::GetInstance();
	common->SetFullScreenGrayscaleOverlayIntensity(intensity * 0.75f);
	common->SetGlitchIntensity(intensity * 0.8f);

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
void SizukuSpecialQuantum::Draw(Camera* camera) {
	main_->Draw(camera);
	sub_->Draw(camera);
}
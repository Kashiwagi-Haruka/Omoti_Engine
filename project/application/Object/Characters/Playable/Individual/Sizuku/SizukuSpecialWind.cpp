#define NOMINMAX
#include "SizukuSpecialWind.h"
#include "SizukuSpecial.h"

void SizukuSpecial::StartWindSpecial() {
	// 風属性はプレイヤーを中心に巻き上がる竜巻。
	animationTimeMax_ = 4.5f;
	particleTransform_ = sizukuTransform_;
	damagePosition_ = sizukuTransform_.translate;
	damageScale_ = {10.0f, 8.0f, 10.0f};
	ConfigureEmitter(*mainEmitter_, {0.25f, 1.0f, 0.55f, 1.0f}, 45, 7.0f, 1.2f);
	mainEmitter_->SetAcceleration({0.0f, 5.0f, 0.0f});
	mainEmitter_->SetFrequency(0.12f);
}

void SizukuSpecial::UpdateWindSpecial(float) {
	particleTransform_.translate = sizukuTransform_.translate;
	mainEmitter_->Update(particleTransform_);
	damagePosition_ = sizukuTransform_.translate;
}

void SizukuSpecialWind::Start(SizukuSpecial& special) { special.StartWindSpecial(); }

void SizukuSpecialWind::Update(SizukuSpecial& special, float deltaTime) { special.UpdateWindSpecial(deltaTime); }
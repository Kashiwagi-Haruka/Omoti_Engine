#define NOMINMAX
#include "SizukuSpecialThunder.h"
#include "SizukuSpecial.h"
#include "Function.h"
#include <cmath>

void SizukuSpecial::StartThunderSpecial() {
	// 雷属性は正面へ高速で一発だけ発射する貫通弾。
	animationTimeMax_ = 2.4f;
	thunderProjectileTransform_.scale = {1.2f, 1.2f, 3.5f};
	thunderProjectileTransform_.rotate = sizukuTransform_.rotate;
	thunderProjectileTransform_.translate = sizukuTransform_.translate;
	thunderProjectileTransform_.translate.y += 1.0f;
	damagePosition_ = thunderProjectileTransform_.translate;
	damageScale_ = {2.0f, 2.0f, 4.0f};
	ConfigureEmitter(*mainEmitter_, {0.75f, 0.55f, 1.0f, 1.0f}, 80, 15.0f, 0.7f);
	mainEmitter_->SetFrequency(0.2f);
}

void SizukuSpecial::UpdateThunderSpecial(float deltaTime) {
	const float yaw = sizukuTransform_.rotate.y;
	const Vector3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
	if (elapsedTime_ >= 0.45f) {
		thunderProjectileTransform_.translate = thunderProjectileTransform_.translate + forward * (42.0f * deltaTime);
		damagePosition_ = thunderProjectileTransform_.translate;
		thunderProjectile_->SetCamera(camera_);
		thunderProjectile_->SetTransform(thunderProjectileTransform_);
		thunderProjectile_->Update();
		particleTransform_ = thunderProjectileTransform_;
		mainEmitter_->Update(particleTransform_);
	}
}

void SizukuSpecialThunder::Start(SizukuSpecial& special) { special.StartThunderSpecial(); }

void SizukuSpecialThunder::Update(SizukuSpecial& special, float deltaTime) { special.UpdateThunderSpecial(deltaTime); }
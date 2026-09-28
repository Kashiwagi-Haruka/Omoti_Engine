#define NOMINMAX
#include "SizukuSpecialThunder.h"
#include "Function.h"
#include "Object3d/Object3dCommon.h"
#include "SizukuSpecial.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace {
constexpr float kChargeDuration = 0.5f;
constexpr float kFieldMaxScale = 4.0f;
constexpr float kFieldForwardOffset = 2.0f;
} // namespace

void SizukuSpecial::StartThunderSpecial() {
	// 雷属性は正面へ高速で一発だけ発射する貫通弾。
	animationTimeMax_ = 2.4f;
	thunderProjectileTransform_.scale = {1.2f, 1.2f, 3.5f};
	thunderProjectileTransform_.rotate = sizukuTransform_.rotate;
	thunderProjectileTransform_.translate = sizukuTransform_.translate;
	thunderProjectileTransform_.translate.y += 1.0f;
	const float yaw = sizukuTransform_.rotate.y;
	const Vector3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
	thunderFieldPlaneTransform_.scale = {};
	thunderFieldPlaneTransform_.rotate = {std::numbers::pi_v<float> / 2.0f, yaw, 0.0f};
	thunderFieldPlaneTransform_.translate = sizukuTransform_.translate + forward * kFieldForwardOffset;
	thunderFieldPlaneTransform_.translate.y -= sizukuHeight_;
	damagePosition_ = thunderProjectileTransform_.translate;
	damageScale_ = {2.0f, 2.0f, 4.0f};
	ConfigureEmitter(*mainEmitter_, {0.75f, 0.55f, 1.0f, 1.0f}, 80, 15.0f, 0.7f);
	mainEmitter_->SetFrequency(0.2f);
}

void SizukuSpecial::UpdateThunderSpecial(float deltaTime) {
	const float yaw = sizukuTransform_.rotate.y;
	const Vector3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
	const float chargeProgress = std::clamp(elapsedTime_ / kChargeDuration, 0.0f, 1.0f);
	thunderFieldPlaneTransform_.scale = {kFieldMaxScale * chargeProgress, kFieldMaxScale * chargeProgress, 1.0f};
	thunderFieldPlaneTransform_.rotate.y = yaw + std::numbers::pi_v<float> * 2.0f * chargeProgress;
	thunderFieldPlaneTransform_.translate = sizukuTransform_.translate + forward * kFieldForwardOffset;
	thunderFieldPlaneTransform_.translate.y -= sizukuHeight_;
	thunderFieldPlane_->SetCamera(camera_);
	thunderFieldPlane_->SetTransform(thunderFieldPlaneTransform_);
	thunderFieldPlane_->Update();

	if (elapsedTime_ >= kChargeDuration) {
		thunderProjectileTransform_.translate = thunderProjectileTransform_.translate + forward * (42.0f * deltaTime);
		damagePosition_ = thunderProjectileTransform_.translate;
		thunderProjectile_->SetCamera(camera_);
		thunderProjectile_->SetTransform(thunderProjectileTransform_);
		thunderProjectile_->Update();
		particleTransform_ = thunderProjectileTransform_;
		mainEmitter_->Update(particleTransform_);
	}
}

void SizukuSpecial::DrawThunderSpecial() {
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	thunderFieldPlane_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
	if (elapsedTime_ >= kChargeDuration)
		thunderProjectile_->Draw();
}

void SizukuSpecialThunder::Start(SizukuSpecial& special) { special.StartThunderSpecial(); }

void SizukuSpecialThunder::Update(SizukuSpecial& special, float deltaTime) { special.UpdateThunderSpecial(deltaTime); }
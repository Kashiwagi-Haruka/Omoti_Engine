#include "SizukuSpecialThunder.h"
#include "Function.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <cmath>
#include <numbers>
namespace {
constexpr float kCharge = 0.75f;
} // namespace
void SizukuSpecialThunder::Initialize() {
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	field_ = std::make_unique<Primitive>();
	field_->Initialize(Primitive::Plane, "Resources/3d/Character/Sizuku/Special/Thunder/sizukuSpecialThunder.png");
	field_->SetEnableLighting(false);

	projectile_ = std::make_unique<Object3d>();
	projectile_->Initialize();
	projectile_->SetEnableLighting(false);
	projectile_->SetModel("sizukuSpecialRain");
	projectile_->SetColor({.85f, .65f, 1, 1});
}
void SizukuSpecialThunder::Start(const SizukuSpecialContext& c) {
	projectileTransform_.scale = {1.2f, 1.2f, 3.5f};
	projectileTransform_.rotate = c.transform.rotate;
	projectileTransform_.translate = c.transform.translate;
	projectileTransform_.translate.y += 1;
	float yaw = c.transform.rotate.y;
	Vector3 f = {std::sin(yaw), 0, std::cos(yaw)};
	fieldTransform_.scale = {};
	projectileTransform_.rotate.y = c.transform.rotate.y + std::numbers::pi_v<float> / 2.0f;
	fieldTransform_.translate = c.transform.translate + f * 2;

	damagePosition_ = projectileTransform_.translate;
	damageScale_ = {2, 2, 4};
}
void SizukuSpecialThunder::Update(const SizukuSpecialContext& c, float dt) {
	float yaw = c.transform.rotate.y;
	Vector3 f = {std::sin(yaw), 0, std::cos(yaw)};
	float p = std::clamp(c.elapsedTime / kCharge, 0.f, 1.f);
	fieldTransform_.scale = {10 * p, 10 * p, 1};
	fieldTransform_.translate = c.transform.translate + f * 2;
	projectileTransform_.rotate.y = c.transform.rotate.y + std::numbers::pi_v<float> / 2.0f;

	field_->SetCamera(c.camera);
	field_->SetTransform(fieldTransform_);
	field_->Update();
	Object3dCommon::GetInstance()->SetFullScreenBinarizationEnabled(true);
	Object3dCommon::GetInstance()->SetBinarizationThreshold(0.35f);
	Object3dCommon::GetInstance()->SetRadialBlurWidth(7.0f);
	Object3dCommon::GetInstance()->SetRadialBlurCenter({0.5f, 0.5f});
	Object3dCommon::GetInstance()->SetRadialBlurSampleCount(5);
	if (c.elapsedTime >= kCharge) {
		projectileTransform_.translate = projectileTransform_.translate + f * (42 * dt);
		damagePosition_ = projectileTransform_.translate;
		projectile_->SetCamera(c.camera);
		projectile_->SetTransform(projectileTransform_);
		projectile_->Update();
		Object3dCommon::GetInstance()->SetFullScreenBinarizationEnabled(false);
		Object3dCommon::GetInstance()->SetRadialBlurWidth(0.0f);
	}
}
void SizukuSpecialThunder::Draw() {
	auto* common = Object3dCommon::GetInstance();
	common->SetBlendMode(BlendMode::kBlendModeAdd);
	common->DrawCommon(Object3dCommon::DrawCommonType::NoCull);
	field_->Draw();
	common->SetBlendMode(BlendMode::kBlendModeAlpha);
	common->DrawCommon();
	projectile_->Draw();
}
#define NOMINMAX
#include "EnemyHPBar.h"
#include "Camera.h"
#include "Function.h"
#include <algorithm>

namespace {
constexpr float kMinHpRatio = 0.0f;
constexpr float kMaxHpRatio = 1.0f;
} // namespace

void EnemyHPBar::Initialize() {
	hpBarPrimitive_ = std::make_unique<Primitive>();
	hpBarPrimitive_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/HPBar/Enemy/HP.png");
	hpBarPrimitive_->SetEnableLighting(false);
	hpBarPrimitive_->SetCamera(camera_);
	ApplyBillboardTransform();
}
void EnemyHPBar::Update() {
	if (!hpBarPrimitive_) {
		return;
	}
	hpBarPrimitive_->SetCamera(camera_);
	ApplyBillboardTransform();
	hpBarPrimitive_->UpdateCameraMatrices();
}
void EnemyHPBar::Draw() {
	if (!hpBarPrimitive_ || HP_ <= 0) {
		return;
	}
	hpBarPrimitive_->Draw();
}
void EnemyHPBar::SetHP(int HP) { HP_ = std::max(0, HP); }
void EnemyHPBar::SetMaxHP(int maxHP) { maxHP_ = std::max(1, maxHP); }
void EnemyHPBar::SetPosition(const Vector3& position) { position_ = position; }
void EnemyHPBar::SetCamera(Camera* camera) { camera_ = camera; }

void EnemyHPBar::ApplyBillboardTransform() {
	if (!hpBarPrimitive_) {
		return;
	}

	const float hpRatio = std::clamp(static_cast<float>(HP_) / static_cast<float>(maxHP_), kMinHpRatio, kMaxHpRatio);
	const float damageRatio = 1.0f - hpRatio;
	const Vector3 scale = {width_, height_, 1.0f};
	Vector3 translate = position_;
	translate.y += verticalOffset_;
	hpBarPrimitive_->SetUvTransform({0.5f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {damageRatio, 0.0f, 0.0f});

	if (!camera_) {
		hpBarPrimitive_->SetTransform({
		    scale, {0.0f, 0.0f, 0.0f},
             translate
        });
		return;
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(scale);
	const Matrix4x4 translateMatrix = Function::MakeTranslateMatrix(translate);
	const Matrix4x4 worldMatrix = Function::Multiply(Function::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
	hpBarPrimitive_->SetWorldMatrix(worldMatrix);
}
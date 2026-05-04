#define NOMINMAX
#include "Damage.h"
#include "Camera.h"
#include "Function.h"
#include <algorithm>
#include <string>

void Damage::Initialize(Camera* camera) {
	camera_ = camera;
	for (int i = 0; i < kDigitCount; ++i) {
		digitPrimitives_[i] = std::make_unique<Primitive>();
		digitPrimitives_[i]->Initialize(Primitive::Plane, "Resources/2d/No.png");
		digitPrimitives_[i]->SetEnableLighting(false);
		digitPrimitives_[i]->SetCamera(camera_);
	}
	SetDamageValue(0);
}

void Damage::SetDamageValue(int damage) {
	damage = std::clamp(damage, 0, MaxDamage_);
	const std::string damageString = std::to_string(damage);
	digits_.clear();
	digits_.reserve(damageString.size());
	for (char c : damageString) {
		digits_.push_back(c - '0');
	}
	isVisible_ = !digits_.empty();
}

void Damage::SetPosition(const Vector3& position) { transform_.translate = position; }

void Damage::Update() {
	if (!isVisible_ || !camera_) {
		return;
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	const int digitCount = static_cast<int>(std::min(digits_.size(), digitPrimitives_.size()));
	const float totalWidth = (digitCount - 1) * digitSpacing_;
	for (int i = 0; i < digitCount; ++i) {
		Primitive* primitive = digitPrimitives_[i].get();
		primitive->SetCamera(camera_);
		primitive->SetUvTransform({0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.1f * static_cast<float>(digits_[i]), 0.0f, 0.0f});

		Transform digitTransform = transform_;
		digitTransform.translate.x += (static_cast<float>(i) * digitSpacing_) - (totalWidth * 0.5f);
		const Matrix4x4 worldMatrix = Function::Multiply(billboardMatrix, Function::MakeAffineMatrix(digitTransform.scale, digitTransform.rotate, digitTransform.translate));
		primitive->SetWorldMatrix(worldMatrix);
		primitive->UpdateCameraMatrices();
	}
}

void Damage::Draw() {
	if (!isVisible_) {
		return;
	}

	const int digitCount = static_cast<int>(std::min(digits_.size(), digitPrimitives_.size()));
	for (int i = 0; i < digitCount; ++i) {
		digitPrimitives_[i]->Draw();
	}
}
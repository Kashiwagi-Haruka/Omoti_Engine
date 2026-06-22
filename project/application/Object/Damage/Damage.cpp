#define NOMINMAX
#include "Damage.h"
#include "Camera.h"
#include "Function.h"
#include <algorithm>
#include <string>
#include "Data/Color.h"

void Damage::Initialize(Camera* camera) {
	camera_ = camera;
	for (int i = 0; i < kDigitCount; ++i) {
		digitPrimitives_[i] = std::make_unique<Primitive>();
		digitPrimitives_[i]->Initialize(Primitive::Plane, "Resources/2d/No.png");
		digitPrimitives_[i]->SetEnableLighting(false);
		digitPrimitives_[i]->SetCamera(camera_);
	}
	SetDamageValue(0);
	isVisible_ = false;
	alpha_ = 0.0f;
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
	if (isVisible_) {
		alpha_ = 1.0f;
		isFading_ = false;
		timer_ = kShowDuration_;
	}
}

void Damage::SetPosition(const Vector3& position) { transform_.translate = position; }

void Damage::Update() {
	if (!isVisible_ || !camera_) {
		return;
	}

	if (timer_ > 0.0f) {
		timer_ -= 1.0f / 60.0f;
		if (timer_ <= 0.0f) {
			if (isFading_) {
				isVisible_ = false;
				alpha_ = 0.0f;
				isFading_ = false;
			} else {
				isFading_ = true;
				timer_ = kFadeDuration_;
			}
		} else if (isFading_) {
			alpha_ = std::clamp(timer_ / kFadeDuration_, 0.0f, 1.0f);
		}
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	const int digitCount = static_cast<int>(std::min(digits_.size(), digitPrimitives_.size()));
	const float totalWidth = (digitCount - 1) * digitSpacing_;
	const Matrix4x4 baseTranslateMatrix = Function::MakeTranslateMatrix(transform_.translate);
	for (int i = 0; i < digitCount; ++i) {
		Primitive* primitive = digitPrimitives_[i].get();
		primitive->SetCamera(camera_);
		switch (attribute_) {
		case Attribute::None:
			primitive->SetColor(Color::RGBAToVector4(255, 255, 255, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Fire:
			primitive->SetColor(Color::RGBAToVector4(240, 40, 40, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Ice:
			primitive->SetColor(Color::RGBAToVector4(130, 210, 240, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Wind:
			primitive->SetColor(Color::RGBAToVector4(100, 210, 100, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Thunder:
			primitive->SetColor(Color::RGBAToVector4(200, 110, 210, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Imaginary:
			primitive->SetColor(Color::RGBAToVector4(240, 225, 70, static_cast<int>(alpha_ * 255)));
			break;
		case Attribute::Quantum:
			primitive->SetColor(Color::RGBAToVector4(85, 85, 220, static_cast<int>(alpha_ * 255)));
			break;
		default:
			break;
		}
		primitive->SetUvTransform({0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.1f * static_cast<float>(digits_[i]), 0.0f, 0.0f});

		const float localXOffset = (static_cast<float>(i) * digitSpacing_) - (totalWidth * 0.5f);
		const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(transform_.scale);
		const Matrix4x4 localOffsetMatrix = Function::MakeTranslateMatrix(localXOffset, 0.0f, 0.0f);
		const Matrix4x4 localBillboardMatrix = Function::Multiply(scaleMatrix, Function::Multiply(localOffsetMatrix, billboardMatrix));
		const Matrix4x4 worldMatrix = Function::Multiply(localBillboardMatrix, baseTranslateMatrix);
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

void Damage::SetAttribute(Attribute attribute) { attribute_ = attribute; }
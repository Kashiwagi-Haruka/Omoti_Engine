#define NOMINMAX
#include "Damage.h"
#include "Camera.h"
#include "Data/Color.h"
#include "Function.h"
#include <algorithm>
#include <string>

namespace {
constexpr int kNormalColor = 180;
constexpr int kCriticalColor = 255;
constexpr int kDamagePositionOffsetCount = 6;
constexpr std::array<Vector3, kDamagePositionOffsetCount> kDamagePositionOffsets = {
    Vector3{0.0f,   0.0f, 0.0f  },
    Vector3{0.25f,  0.2f, 0.0f  },
    Vector3{-0.25f, 0.4f, 0.0f  },
    Vector3{0.0f,   0.6f, 0.25f },
    Vector3{0.25f,  0.8f, -0.25f},
    Vector3{-0.25f, 1.0f, 0.25f },
};

float SmoothStep(float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}
const char* GetAttributeName(Attribute attribute) {
	switch (attribute) {
	case Attribute::Fire:
		return "Fire";
	case Attribute::Ice:
		return "Ice";
	case Attribute::Wind:
		return "Wind";
	case Attribute::Thunder:
		return "Thunder";
	case Attribute::Imaginary:
		return "Imaginary";
	case Attribute::Quantum:
		return "Quantum";
	case Attribute::None:
	case Attribute::MAXATTRIBUTE:
	default:
		return nullptr;
	}
}

std::string GetDamageNumberTexturePath(Attribute attribute, Attribute reactionPreviousAttribute) {
	const char* attributeName = GetAttributeName(attribute);
	if (!attributeName) {
		return "Resources/2d/Attribute/Numbers/No.png";
	}

	const char* previousAttributeName = GetAttributeName(reactionPreviousAttribute);
	if (previousAttributeName && reactionPreviousAttribute != attribute) {
		return std::string("Resources/2d/Attribute/Numbers/") + previousAttributeName + "Reaction/No" + attributeName + ".png";
	}

	return std::string("Resources/2d/Attribute/Numbers/No") + attributeName + ".png";
}
}


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
	const bool wasVisible = isVisible_;
	damage = std::clamp(damage, 0, MaxDamage_);
	const std::string damageString = std::to_string(damage);
	digits_.clear();
	digits_.reserve(damageString.size());
	for (char c : damageString) {
		digits_.push_back(c - '0');
	}
	isVisible_ = !digits_.empty();
	if (isVisible_) {
		if (wasVisible) {
			damagePositionOffsetIndex_ = (damagePositionOffsetIndex_ + 1) % static_cast<int>(kDamagePositionOffsets.size());
		} else {
			damagePositionOffsetIndex_ = 0;
		}
		damagePositionOffset_ = kDamagePositionOffsets[damagePositionOffsetIndex_];
		transform_.translate = basePosition_ + damagePositionOffset_;
		alpha_ = kAppearStartAlpha_;
		appearTimer_ = 0.0f;
		isFading_ = false;
		timer_ = kShowDuration_;
	}
}

void Damage::SetPosition(const Vector3& position) {
	basePosition_ = position;
	transform_.translate = basePosition_ + damagePositionOffset_;
}
void Damage::Update() {
	if (!isVisible_ || !camera_) {
		return;
	}

	if (!isFading_ && appearTimer_ < kAppearDuration_) {
		appearTimer_ = std::min(appearTimer_ + 1.0f / 60.0f, kAppearDuration_);
		const float appearEase = SmoothStep(appearTimer_ / kAppearDuration_);
		alpha_ = std::clamp(kAppearStartAlpha_ + (1.0f - kAppearStartAlpha_) * appearEase, 0.0f, 1.0f);
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
	const float appearEase = SmoothStep(appearTimer_ / kAppearDuration_);
	const float scaleMultiplier = !isFading_ ? kAppearStartScale_ + (1.0f - kAppearStartScale_) * appearEase : 1.0f;
	const Vector3 drawScale = {transform_.scale.x * scaleMultiplier, transform_.scale.y * scaleMultiplier, transform_.scale.z * scaleMultiplier};
	const float totalWidth = (digitCount - 1) * digitSpacing_;
	const Matrix4x4 baseTranslateMatrix = Function::MakeTranslateMatrix(transform_.translate);
	for (int i = 0; i < digitCount; ++i) {
		Primitive* primitive = digitPrimitives_[i].get();
		primitive->SetCamera(camera_);
		if (isCritical_) {
			primitive->SetColor(Color::RGBAToVector4(kCriticalColor, kCriticalColor, kCriticalColor, static_cast<int>(alpha_ * 255)));
		} else {
			primitive->SetColor(Color::RGBAToVector4(kNormalColor, kNormalColor, kNormalColor, static_cast<int>(alpha_ * 255)));
		}
		primitive->SetTexturePath(GetDamageNumberTexturePath(attribute_, reactionPreviousAttribute_));
		primitive->SetUvTransform({0.1f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.1f * static_cast<float>(digits_[i]), 0.0f, 0.0f});

		const float localXOffset = (static_cast<float>(i) * digitSpacing_) - (totalWidth * 0.5f);
		const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(drawScale);
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

void Damage::SetAttribute(Attribute attribute) {
	attribute_ = attribute;
	reactionPreviousAttribute_ = Attribute::None;
}

void Damage::SetReactionAttribute(Attribute previousAttribute, Attribute appliedAttribute) {
	attribute_ = appliedAttribute;
	reactionPreviousAttribute_ = previousAttribute;
}
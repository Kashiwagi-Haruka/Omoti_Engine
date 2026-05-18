#define NOMINMAX
#include "Adhesion.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Engine/base/GameBase.h"
namespace {
	const float kAppearanceDuration = 0.5f; // 出現アニメーションの継続時間
const float kComparisonDisplayDuration = 1.0f; // 属性比較表示の継続時間
}
Adhesion::Adhesion() {
	preAttributePlane_ = std::make_unique<Primitive>();
	AttributePlane_ = std::make_unique<Primitive>();
	AttributeReactionPlane_ = std::make_unique<Primitive>();
}

Adhesion::~Adhesion() = default;

void Adhesion::Initialize() {
	preAttributePlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Attribute/Fire.png");
	preAttributePlane_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
	preAttributePlane_->SetEnableLighting(false);
	AttributePlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Attribute/Fire.png");
	AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	AttributePlane_->SetEnableLighting(false);
	AttributeReactionPlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Attribute/AttributeReaction/fireReaction.png");
	AttributeReactionPlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	AttributeReactionPlane_->SetEnableLighting(false);
}

uint32_t Adhesion::ResolveTextureIndex(Attribute attribute) const {
	const char* texturePath = "Resources/2d/Attribute/Fire.png";
	switch (attribute) {
	case Attribute::Fire:
		texturePath = "Resources/2d/Attribute/Fire.png";
		break;
	case Attribute::Ice:
		texturePath = "Resources/2d/Attribute/Ice.png";
		break;
	case Attribute::Wind:
		texturePath = "Resources/2d/Attribute/Wind.png";
		break;
	case Attribute::Thunder:
		texturePath = "Resources/2d/Attribute/Tunder.png";
		break;
	case Attribute::Imaginary:
		texturePath = "Resources/2d/Attribute/Imaginary.png";
		break;
	case Attribute::Quantum:
		texturePath = "Resources/2d/Attribute/Quantum.png";
		break;
	case Attribute::None:
	case Attribute::MAXATTRIBUTE:
	default:
		break;
	}
	return TextureManager::GetInstance()->GetTextureIndexByfilePath(texturePath);
}

void Adhesion::RefreshAttributeTexture() {
	if (currentAttribute_ == Attribute::None) {
		return;
	}
	AttributePlane_->SetTextureIndex(ResolveTextureIndex(currentAttribute_));
	AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
}

void Adhesion::RefreshComparisonTransform() {
	if (!hasBaseTransform_) {
		return;
	}

	Transform leftTransform = baseTransform_;
	leftTransform.translate.x -= 0.5f;
	leftTransform.translate.y += 1.75f;
	leftTransform.scale = {0.6f, 0.6f, 0.6f};
	preAttributePlane_->SetTransform(leftTransform);

	Transform rightTransform = baseTransform_;
	rightTransform.translate.x += 0.5f;
	rightTransform.translate.y += 1.75f;
	rightTransform.scale = {0.6f, 0.6f, 0.6f};
	AttributePlane_->SetTransform(rightTransform);
}

void Adhesion::SetTransform(const Transform& transform) {
	baseTransform_ = transform;
	hasBaseTransform_ = true;
	if (isComparisonDisplayActive_) {
		RefreshComparisonTransform();
		return;
	}

	Transform uiTransform = transform;
	uiTransform.translate.y += 1.75f;
	uiTransform.scale = {0.8f, 0.8f, 0.8f};
	AttributePlane_->SetTransform(uiTransform);
}

void Adhesion::AddAttribute(Attribute attribute) {
	if (attribute == Attribute::None || attribute == Attribute::MAXATTRIBUTE) {
		return;
	}
	const uint32_t bit = (1u << static_cast<uint32_t>(attribute));
	const Attribute previousAttribute = currentAttribute_;
	const bool hadPreviousAttribute = (previousAttribute != Attribute::None) && (previousAttribute != attribute);
	attributeBitMask_ |= bit;
	currentAttribute_ = attribute;

	if (hadPreviousAttribute) {
		preAttributePlane_->SetTextureIndex(ResolveTextureIndex(previousAttribute));
		preAttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		AttributePlane_->SetTextureIndex(ResolveTextureIndex(currentAttribute_));
		AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		isComparisonDisplayActive_ = true;
		comparisonDisplayTimer_ = kComparisonDisplayDuration;
		RefreshComparisonTransform();
		return;
	}

	isComparisonDisplayActive_ = false;
	comparisonDisplayTimer_ = 0.0f;
	RefreshAttributeTexture();
}

void Adhesion::Update() {
	if (attributeBitMask_ == 0) {
		return;
	}

	if (isComparisonDisplayActive_) {
		comparisonDisplayTimer_ -= GameBase::GetInstance()->GetDeltaTime();
		preAttributePlane_->Update();
		AttributePlane_->Update();
		if (comparisonDisplayTimer_ <= 0.0f) {
			isComparisonDisplayActive_ = false;
			attributeBitMask_ = 0;
		}
		return;
	}

	float alpha = AttributePlane_->GetColor().w;
	if (alpha < 1.0f) {
		alpha += GameBase::GetInstance()->GetDeltaTime() / kAppearanceDuration;
		alpha = std::min(alpha, 1.0f);
		AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	}
	AttributePlane_->Update();
}

void Adhesion::Draw() {
	if (attributeBitMask_ == 0) {
		return;
	}
	if (isComparisonDisplayActive_) {
		preAttributePlane_->Draw();
		AttributePlane_->Draw();
		return;
	}
}
#include "Adhesion.h"
#include "Camera.h"
#include "TextureManager.h"

Adhesion::Adhesion() {
	preAttributePlane_ = std::make_unique<Primitive>();
	AttributePlane_ = std::make_unique<Primitive>();
}

Adhesion::~Adhesion() = default;

void Adhesion::Initialize() {
	preAttributePlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Attribute/Fire.png");
	preAttributePlane_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
	preAttributePlane_->SetEnableLighting(false);
	AttributePlane_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Attribute/Fire.png");
	AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	AttributePlane_->SetEnableLighting(false);
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
}

void Adhesion::SetTransform(const Transform& transform) {
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
	attributeBitMask_ |= bit;
	currentAttribute_ = attribute;
	RefreshAttributeTexture();
}

void Adhesion::Update() {
	if (attributeBitMask_ == 0) {
		return;
	}
	AttributePlane_->Update();
}

void Adhesion::Draw() {
	if (attributeBitMask_ == 0) {
		return;
	}
	AttributePlane_->Draw();
}
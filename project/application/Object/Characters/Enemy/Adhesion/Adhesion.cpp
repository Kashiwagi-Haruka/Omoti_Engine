#define NOMINMAX
#include "Adhesion.h"
#include "Camera.h"
#include "Engine/base/GameBase.h"
#include "Function.h"
#include "Object3d/Object3dCommon.h"
#include "TextureManager.h"
namespace {
const float kAppearanceDuration = 0.5f;      // 出現アニメーションの継続時間
const float kReactionDisplayDuration = 3.0f; // 属性リアクション表示の継続時間
const float kReactionCutSize = 200.0f;       // リアクション画像を切り取るサイズ
} // namespace
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

uint32_t Adhesion::ResolveReactionTextureIndex(Attribute appliedAttribute) const {
	const char* texturePath = "Resources/2d/Attribute/AttributeReaction/fireReaction.png";
	switch (appliedAttribute) {
	case Attribute::Fire:
		texturePath = "Resources/2d/Attribute/AttributeReaction/fireReaction.png";
		break;
	case Attribute::Ice:
		texturePath = "Resources/2d/Attribute/AttributeReaction/IceReaction.png";
		break;
	case Attribute::Thunder:
		texturePath = "Resources/2d/Attribute/AttributeReaction/ThunderReaction.png";
		break;
	case Attribute::Wind:
		texturePath = "Resources/2d/Attribute/AttributeReaction/WindReaction.png";
		break;
	case Attribute::Imaginary:
		texturePath = "Resources/2d/Attribute/AttributeReaction/ImaginaryReaction.png";
		break;
	case Attribute::Quantum:
		texturePath = "Resources/2d/Attribute/AttributeReaction/QuatumReaction.png";
		break;
	case Attribute::None:
	case Attribute::MAXATTRIBUTE:
	default:
		break;
	}
	return TextureManager::GetInstance()->GetTextureIndexByfilePath(texturePath);
}

int Adhesion::ResolveReactionFrameIndex(Attribute appliedAttribute, Attribute previousAttribute) const {
	if (appliedAttribute == previousAttribute || appliedAttribute == Attribute::None || appliedAttribute == Attribute::MAXATTRIBUTE) {
		return 0;
	}

	int frameIndex = 0;
	const Attribute reactionOrder[] = {Attribute::Fire, Attribute::Ice, Attribute::Thunder, Attribute::Wind, Attribute::Imaginary, Attribute::Quantum};
	for (Attribute candidate : reactionOrder) {
		if (candidate == appliedAttribute) {
			continue;
		}
		if (candidate == previousAttribute) {
			return frameIndex;
		}
		++frameIndex;
	}

	return 0;
}
void Adhesion::RefreshAttributeTexture() {
	if (currentAttribute_ == Attribute::None) {
		return;
	}
	AttributePlane_->SetTextureIndex(ResolveTextureIndex(currentAttribute_));
	AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
}

void Adhesion::RefreshReactionTexture(Attribute appliedAttribute, Attribute previousAttribute) {
	const uint32_t textureIndex = ResolveReactionTextureIndex(appliedAttribute);
	AttributeReactionPlane_->SetTextureIndex(textureIndex);

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex);
	const float textureWidth = metadata.width > 0 ? static_cast<float>(metadata.width) : kReactionCutSize;
	const float textureHeight = metadata.height > 0 ? static_cast<float>(metadata.height) : kReactionCutSize;
	const int frameIndex = ResolveReactionFrameIndex(appliedAttribute, previousAttribute);
	const float uvWidth = kReactionCutSize / textureWidth;
	const float uvHeight = kReactionCutSize / textureHeight;
	const float uvLeft = static_cast<float>(frameIndex) * kReactionCutSize / textureWidth;

	AttributeReactionPlane_->SetUvTransform({uvWidth, uvHeight, 1.0f}, {0.0f, 0.0f, 0.0f}, {uvLeft, 0.0f, 0.0f});
	AttributeReactionPlane_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
}

void Adhesion::ApplyBillboardTransform(Primitive* primitive, const Transform& transform) const {
	if (!primitive) {
		return;
	}

	if (!camera_) {
		primitive->SetTransform(transform);
		return;
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(transform.scale);
	const Matrix4x4 translateMatrix = Function::MakeTranslateMatrix(transform.translate);
	const Matrix4x4 worldMatrix = Function::Multiply(Function::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
	primitive->SetWorldMatrix(worldMatrix);
}

void Adhesion::RefreshComparisonTransform() {
	if (!hasBaseTransform_) {
		return;
	}

	Transform leftTransform = baseTransform_;
	leftTransform.translate.x -= 0.7f;
	leftTransform.translate.y += 1.75f;
	leftTransform.scale = {0.6f, 0.6f, 0.6f};
	ApplyBillboardTransform(preAttributePlane_.get(), leftTransform);

	Transform reactionTransform = baseTransform_;
	reactionTransform.translate.y += 1.75f;
	reactionTransform.scale = {0.6f, 0.6f, 0.6f};
	ApplyBillboardTransform(AttributeReactionPlane_.get(), reactionTransform);

	Transform rightTransform = baseTransform_;
	rightTransform.translate.x += 0.7f;
	rightTransform.translate.y += 1.75f;
	rightTransform.scale = {0.6f, 0.6f, 0.6f};
	ApplyBillboardTransform(AttributePlane_.get(), rightTransform);
}

void Adhesion::SetTransform(const Transform& transform) {
	baseTransform_ = transform;
	hasBaseTransform_ = true;
	if (isComparisonDisplayActive_) {
		RefreshComparisonTransform();
	}

	Transform uiTransform = transform;
	uiTransform.translate.y += 1.75f;
	uiTransform.scale = {0.8f, 0.8f, 0.8f};
	ApplyBillboardTransform(AttributePlane_.get(), uiTransform);
}

void Adhesion::SetCamera(Camera* camera) {
	camera_ = camera;
	preAttributePlane_->SetCamera(camera_);
	AttributePlane_->SetCamera(camera_);
	AttributeReactionPlane_->SetCamera(camera_);
}

bool Adhesion::AddAttribute(Attribute attribute) {
	if (attribute == Attribute::None || attribute == Attribute::MAXATTRIBUTE) {
		return false;
	}
	const uint32_t bit = (1u << static_cast<uint32_t>(attribute));
	if (currentAttribute_ == attribute && (attributeBitMask_ & bit) != 0) {
		return false;
	}
	const Attribute previousAttribute = currentAttribute_;
	const bool hadPreviousAttribute = (previousAttribute != Attribute::None) && (previousAttribute != attribute);
	attributeBitMask_ |= bit;
	currentAttribute_ = attribute;

	if (hadPreviousAttribute) {
		RefreshReactionTexture(currentAttribute_, previousAttribute);
		attributeBitMask_ = 0;
		currentAttribute_ = Attribute::None;
		preAttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
		AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
		isComparisonDisplayActive_ = true;
		comparisonDisplayTimer_ = kReactionDisplayDuration;
		RefreshComparisonTransform();
		return true;
	}

	isComparisonDisplayActive_ = false;
	comparisonDisplayTimer_ = 0.0f;
	RefreshAttributeTexture();
	return false;
}

void Adhesion::Update() {
	if (isComparisonDisplayActive_) {
		comparisonDisplayTimer_ -= GameBase::GetInstance()->GetDeltaTime();
		RefreshComparisonTransform();
		AttributeReactionPlane_->Update();
		if (comparisonDisplayTimer_ <= 0.0f) {
			isComparisonDisplayActive_ = false;
			comparisonDisplayTimer_ = 0.0f;
			AttributeReactionPlane_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
			SetTransform(baseTransform_);
		}
		return;
	}

	if (attributeBitMask_ == 0) {
		return;
	}

	float alpha = AttributePlane_->GetColor().w;
	if (alpha < 1.0f) {
		alpha += GameBase::GetInstance()->GetDeltaTime() / kAppearanceDuration;
		alpha = std::min(alpha, 1.0f);
		AttributePlane_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	}
	SetTransform(baseTransform_);
	AttributePlane_->Update();
}

void Adhesion::Draw() {
	Object3dCommon::GetInstance()->DrawCommonNoCull();
	if (isComparisonDisplayActive_) {
		AttributeReactionPlane_->Draw();
		return;
	}
	if (attributeBitMask_ == 0) {
		return;
	}

	AttributePlane_->Draw();
	Object3dCommon::GetInstance()->DrawCommon();
}
#pragma once
#include "Mesh/Primitive/Primitive.h"
#include "Object/Characters/Base/Attribute.h"
#include <cstdint>
#include <memory>
class Camera;

class Adhesion {

	std::unique_ptr<Primitive> preAttributePlane_;
	std::unique_ptr<Primitive> AttributePlane_;
	std::unique_ptr<Primitive> AttributeReactionPlane_;
	uint32_t attributeBitMask_ = 0;
	Attribute currentAttribute_ = Attribute::None;
	bool isComparisonDisplayActive_ = false;
	float comparisonDisplayTimer_ = 0.0f;
	Transform baseTransform_{};
	bool hasBaseTransform_ = false;
	Camera* camera_ = nullptr;
	uint32_t ResolveTextureIndex(Attribute attribute) const;
	uint32_t ResolveReactionTextureIndex(Attribute appliedAttribute) const;
	int ResolveReactionFrameIndex(Attribute appliedAttribute, Attribute previousAttribute) const;
	void RefreshAttributeTexture();
	void RefreshReactionTexture(Attribute appliedAttribute, Attribute previousAttribute);
	void RefreshComparisonTransform();
	void ApplyBillboardTransform(Primitive* primitive, const Transform& transform) const;

public:
	Adhesion();
	~Adhesion();
	void Initialize();
	void Update();
	void SetTransform(const Transform& transform);
	void SetCamera(Camera* camera);
	bool AddAttribute(Attribute attribute);
	void Draw();
};
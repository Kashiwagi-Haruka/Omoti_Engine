#pragma once
#include "Mesh/Primitive/Primitive.h"
#include "Object/Characters/Base/Attribute.h"
#include <cstdint>
#include <memory>
class Adhesion {

	std::unique_ptr<Primitive> preAttributePlane_;
	std::unique_ptr<Primitive> AttributePlane_;
	uint32_t attributeBitMask_ = 0;
	Attribute currentAttribute_ = Attribute::None;

	uint32_t ResolveTextureIndex(Attribute attribute) const;
	void RefreshAttributeTexture();

public:
	Adhesion();
	~Adhesion();
	void Initialize();
	void Update();
	void SetTransform(const Transform& transform);
	void AddAttribute(Attribute attribute);
	void Draw();
};
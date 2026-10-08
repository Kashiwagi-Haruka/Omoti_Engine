#pragma once
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "ParticleEmitter.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialAttributeBase.h"
#include <memory>
class SizukuSpecialImaginary final : public SizukuSpecialAttributeBase {
	std::unique_ptr<Primitive> field_;
	std::unique_ptr<ParticleEmitter> emitter_;
	Transform fieldTransform_{}, particleTransform_{};
	Vector3 damagePosition_{}, damageScale_{};
	bool emitted_ = false;

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw(Camera* camera) override;
	float GetDuration() const override { return 4; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
};
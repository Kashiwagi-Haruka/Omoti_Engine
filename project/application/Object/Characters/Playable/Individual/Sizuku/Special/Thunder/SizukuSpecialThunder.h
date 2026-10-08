#pragma once
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "ParticleEmitter.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialAttributeBase.h"
#include <memory>

class SizukuSpecialThunder final : public SizukuSpecialAttributeBase {
	std::unique_ptr<Primitive> field_;
	std::unique_ptr<Object3d> projectile_;
	std::unique_ptr<ParticleEmitter> particleEmitter_;
	Transform fieldTransform_{}, projectileTransform_{}, particleTransform_{};
	Vector3 damagePosition_{}, damageScale_{};

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw(Camera* camera) override;
	float GetDuration() const override { return 2.4f; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
};
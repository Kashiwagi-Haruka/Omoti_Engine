#pragma once
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialAttributeBase.h"
#include <memory>
class SizukuSpecialThunder final : public SizukuSpecialAttributeBase {
	std::unique_ptr<Primitive> field_;
	std::unique_ptr<Object3d> projectile_;
	Transform fieldTransform_{}, projectileTransform_{};
	Vector3 damagePosition_{}, damageScale_{};

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw() override;
	float GetDuration() const override { return 2.4f; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
};
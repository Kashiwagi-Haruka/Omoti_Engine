#pragma once
#include "ParticleEmitter.h"
#include "SizukuSpecialAttributeBase.h"
#include <memory>
class SizukuSpecialWind final : public SizukuSpecialAttributeBase {
	std::unique_ptr<ParticleEmitter> emitter_;
	Transform particleTransform_{};
	Vector3 damagePosition_{}, damageScale_{};

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw() override;
	float GetDuration() const override { return 4.5f; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
};
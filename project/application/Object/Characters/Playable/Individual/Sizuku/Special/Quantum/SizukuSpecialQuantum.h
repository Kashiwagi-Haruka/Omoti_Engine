#pragma once
#include "ParticleEmitter.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialAttributeBase.h"
#include <memory>
class SizukuSpecialQuantum final : public SizukuSpecialAttributeBase {
	std::unique_ptr<ParticleEmitter> main_, sub_;
	Transform origin_{};
	Vector3 damagePosition_{}, damageScale_{};
	bool emitted_ = false;

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw() override;
	float GetDuration() const override { return 4.2f; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
};
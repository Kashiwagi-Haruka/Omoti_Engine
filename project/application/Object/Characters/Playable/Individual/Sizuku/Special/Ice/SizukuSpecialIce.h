#pragma once
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialAttributeBase.h"
#include <memory>
#include <random>
class SizukuSpecialIce final : public SizukuSpecialAttributeBase {
	std::unique_ptr<Primitive> field_;
	std::unique_ptr<Object3d> dome_, flower_;
	std::vector<std::unique_ptr<Object3d>> rains_;
	std::vector<Transform> rainTransforms_;
	Transform fieldTransform_{}, domeTransform_{}, flowerTransform_{};
	Vector3 damagePosition_{}, damageScale_{};
	std::mt19937 random_{std::random_device{}()};
	Transform player_{};
	void ResetRain(size_t, bool);

public:
	void Initialize() override;
	void Start(const SizukuSpecialContext&) override;
	void Update(const SizukuSpecialContext&, float) override;
	void Draw(Camera* camera) override;
	float GetDuration() const override { return 8; }
	Vector3 GetDamagePosition() const override { return damagePosition_; }
	Vector3 GetDamageScale() const override { return damageScale_; }
	const std::vector<Transform>& GetRainTransforms() const override { return rainTransforms_; }
};
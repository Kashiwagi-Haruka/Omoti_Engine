#pragma once
#include "Object3d/Object3d.h"
#include "Primitive/Primitive.h"
#include "Transform.h"
#include "Vector3.h"
#include <array>
#include <cstddef>
#include <memory>
#include <vector>
class EnemyHitEffect {

	static constexpr std::size_t kParticleCount = 7;

	std::unique_ptr<Object3d> hitEffect_;
	std::array<std::unique_ptr<Primitive>, kParticleCount> hitParticles_;
	std::array<Transform, kParticleCount> hitParticleTransforms_;
	std::array<float, kParticleCount> particleBaseRotations_{};
	std::array<Vector3, kParticleCount> particleBaseScales_{};
	Transform hitTransform_;
	Camera* camera_ = nullptr;
	Vector3 enemyPosition_;
	bool isActive_ = false;
	float activeTimer_ = 0.0f;
	float activeDuration_ = 0.28f;

public:
	void Initialize();
	void SetCamera(Camera* camera) { camera_ = camera; };
	void SetPosition(const Vector3& position) { enemyPosition_ = position; };
	void Activate(const Vector3& position);
	bool IsActive() const { return isActive_; }
	void Update();
	void Draw();
};
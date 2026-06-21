#pragma once
#include "Object3d/Object3d.h"
#include "ParticleEmitter.h"
#include "Transform.h"
#include "Vector3.h"
#include <array>
#include <memory>
#include <string>
class EnemyHitEffect {

	struct IceShard {
		std::unique_ptr<Object3d> object;
		Transform transform;
		Vector3 velocity;
		Vector3 angularVelocity;
		float baseScale;
	};

	static constexpr int kIceShardCount = 10;

	std::unique_ptr<Object3d> hitEffect_;
	std::unique_ptr<ParticleEmitter> hitParticleEmitter_;
	std::array<IceShard, kIceShardCount> iceShards_;
	std::string hitParticleGroupName_;
	Transform hitTransform_;
	Transform hitParticleTransform_;
	Camera* camera_ = nullptr;
	Vector3 enemyPosition_;
	bool isActive_ = false;
	float activeTimer_ = 0.0f;
	float activeDuration_ = 0.35f;

public:
	void Initialize();
	void SetCamera(Camera* camera) { camera_ = camera; };
	void SetPosition(const Vector3& position) { enemyPosition_ = position; };
	void Activate(const Vector3& position);
	bool IsActive() const { return isActive_; }
	void Update();
	void Draw();
};
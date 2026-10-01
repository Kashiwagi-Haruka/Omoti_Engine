#pragma once
#include "ParticleEmitter.h"
#include "Vector3.h"
#include <memory>

/// 回復時にプレイヤーの足元から立ち上るパーティクル。
class Heal {
public:
	void Initialize();
	void Start();
	void Update(const Vector3& playerPosition, float deltaTime);
	void Draw();

private:
	static constexpr float kEmissionDuration_ = 2.0f;
	static constexpr float kEmissionInterval_ = 0.1f;
	std::unique_ptr<ParticleEmitter> emitter_;
	float elapsedTime_ = kEmissionDuration_;
	float emissionTimer_ = 0.0f;
};
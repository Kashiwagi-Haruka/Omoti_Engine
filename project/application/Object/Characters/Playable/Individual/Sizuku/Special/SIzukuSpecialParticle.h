#pragma once
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include <memory>
#include <numbers>

inline std::unique_ptr<ParticleEmitter> CreateSpecialEmitter(const char* groupName) {
	ParticleManager::GetInstance()->CreateParticleGroupIfMissing(groupName, "Resources/2d/defaultParticle.png");
	return std::make_unique<ParticleEmitter>(groupName);
}

inline void ConfigureSpecialEmitter(ParticleEmitter& emitter, const Vector4& color, uint32_t count, float speed, float life) {
	emitter.SetCount(count);
	emitter.SetFrequency(0.0f);
	emitter.SetAcceleration({0.0f, -0.35f, 0.0f});
	emitter.SetAreaMin({-0.35f, -0.35f, -0.35f});
	emitter.SetAreaMax({0.35f, 0.35f, 0.35f});
	emitter.SetEmissionAngle(std::numbers::pi_v<float> * 2.0f);
	emitter.SetEmissionSpeed(speed);
	emitter.SetLife(life);
	emitter.SetBeforeColor(color);
	emitter.SetAfterColor({color.x, color.y, color.z, 0.0f});
}
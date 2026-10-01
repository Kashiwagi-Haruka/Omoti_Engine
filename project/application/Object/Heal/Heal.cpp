#include "Heal.h"
#include "ParticleManager.h"
#include "Object3d/Object3dCommon.h"

void Heal::Initialize() {
	ParticleManager::GetInstance()->CreateParticleGroupIfMissing("heal", "Resources/2d/Heal.png");
	emitter_ = std::make_unique<ParticleEmitter>("heal");
	emitter_->SetCount(3);
	emitter_->SetAreaMin({-0.8f, 0.0f, -0.8f});
	emitter_->SetAreaMax({0.8f, 1.0f, 0.8f});
	emitter_->SetAcceleration({0.0f, 5.0f, 0.0f});
	emitter_->SetEmissionSpeed(1.5f);
	emitter_->SetLife(1.0f);
	emitter_->SetBeforeColor({1.0f, 1.0f, 1.0f, 1.0f});
	emitter_->SetAfterColor({0.45f, 1.0f, 0.55f, 0.0f});
}

void Heal::Start() {
	elapsedTime_ = 0.0f;
	emissionTimer_ = kEmissionInterval_;
}

void Heal::Update(const Vector3& playerPosition, float deltaTime) {
	if (!emitter_ || elapsedTime_ >= kEmissionDuration_) {
		return;
	}

	Transform transform{};
	transform.scale = {0.65f, 0.65f, 0.65f};
	transform.translate = playerPosition;
	transform.translate.y -= 1.0f;
	emitter_->SetTransform(transform);

	emissionTimer_ += deltaTime;
	while (emissionTimer_ >= kEmissionInterval_) {
		emitter_->Emit();
		emissionTimer_ -= kEmissionInterval_;
	}
	elapsedTime_ += deltaTime;
}

void Heal::Draw() {
	if (emitter_) {
		emitter_->Draw();
		Object3dCommon::GetInstance()->DrawCommon();
	}
}

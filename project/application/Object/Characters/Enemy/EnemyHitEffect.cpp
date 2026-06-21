#include "EnemyHitEffect.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "ParticleManager.h"
#include <cstdint>
#include <numbers>
#include <string>

namespace {
constexpr float kFrameTime = 1.0f / 60.0f;
constexpr const char* kParticleTexturePath = "Resources/2d/ArrowParticle.png";
constexpr const char* kParticleGroupPrefix = "enemyHitParticle_";
uint32_t gHitParticleGroupSerial = 0;
} // namespace

void EnemyHitEffect::Initialize() {

	ModelManager::GetInstance()->LoadModel("Resources/3d", "HitEffect");

	hitEffect_ = std::make_unique<Object3d>();
	hitEffect_->Initialize();
	hitEffect_->SetModel("HitEffect");
	hitEffect_->SetEnableLighting(false);
	hitEffect_->SetCamera(camera_);
	hitTransform_ = {
	    .scale{1, 1, 1},
        .rotate{0, 0, 0},
        .translate{0, 0, 0}
    };

	hitParticleGroupName_ = kParticleGroupPrefix + std::to_string(gHitParticleGroupSerial++);
	ParticleManager::GetInstance()->CreateParticleGroup(hitParticleGroupName_, kParticleTexturePath);
	hitParticleTransform_ = {
	    .scale{0.10f, 0.10f, 0.10f},
        .rotate{0.0f,  0.0f,  0.0f },
        .translate{0.0f,  0.0f,  0.0f }
    };
	hitParticleEmitter_ = std::make_unique<ParticleEmitter>(hitParticleGroupName_);
	hitParticleEmitter_->SetTransform(hitParticleTransform_);
	hitParticleEmitter_->SetCount(35);
	hitParticleEmitter_->SetFrequency(1.0f);
	hitParticleEmitter_->SetAcceleration({0.0f, 0.0f, 0.0f});
	hitParticleEmitter_->SetAreaMin({-2.0f, -2.0f, 0.0f});
	hitParticleEmitter_->SetAreaMax({2.0f, 2.0f, 0.0f});
	hitParticleEmitter_->SetLife(activeDuration_);
	hitParticleEmitter_->SetBeforeColor({0.5f, 0.72f, 1.0f, 1.0f});
	hitParticleEmitter_->SetAfterColor({1.0f, 1.0f, 1.0f, 0.0f});
	hitParticleEmitter_->SetEmissionAngle(std::numbers::pi_v<float> * 2.0f);
	hitParticleEmitter_->SetEmissionSpeed(3.2f);

	enemyPosition_ = {0.0f, 0.0f, 0.0f};
}

void EnemyHitEffect::Activate(const Vector3& position) {
	isActive_ = true;
	activeTimer_ = activeDuration_;
	enemyPosition_ = position;

	hitParticleTransform_.translate = enemyPosition_;
	if (hitParticleEmitter_) {
		hitParticleEmitter_->SetTransform(hitParticleTransform_);
		hitParticleEmitter_->Emit();
	}
}

void EnemyHitEffect::Update() {
	if (!isActive_) {
		return;
	}

	activeTimer_ -= kFrameTime;
	if (activeTimer_ <= 0.0f) {
		isActive_ = false;
		return;
	}

	hitTransform_.translate = enemyPosition_;
	hitTransform_.rotate.y = std::numbers::pi_v<float>;

	Matrix4x4 c = camera_->GetWorldMatrix();
	c.m[3][0] = c.m[3][1] = c.m[3][2] = 0;
	Matrix4x4 world = Function::Multiply(c, Function::MakeAffineMatrix(hitTransform_.scale, hitTransform_.rotate, hitTransform_.translate));
	ParticleManager::GetInstance()->SetCamera(camera_);
	hitEffect_->SetCamera(camera_);
	hitEffect_->SetWorldMatrix(world);
	hitEffect_->Update();
}

void EnemyHitEffect::Draw() {
	if (!isActive_) {
		return;
	}

	hitEffect_->Draw();
	if (hitParticleEmitter_) {
		hitParticleEmitter_->Draw();
		Object3dCommon::GetInstance()->DrawCommonNoCullDepth();
	}
}
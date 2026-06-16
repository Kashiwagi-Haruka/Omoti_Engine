#include "EnemyHitEffect.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include <numbers>

namespace {
constexpr float kFrameTime = 1.0f / 60.0f;
constexpr const char* kParticleTexturePath = "Resources/2d/defaultParticle.png";
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

	constexpr float kBaseRotations[kParticleCount] = {
	    0.0f, 0.42f, 0.96f, 1.58f, 2.24f, 2.86f, 3.44f,
	};
	constexpr Vector3 kBaseScales[kParticleCount] = {
	    {1.15f, 1.15f, 1.0f},
        {0.82f, 0.82f, 1.0f},
        {1.42f, 1.42f, 1.0f},
        {0.64f, 0.64f, 1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.28f, 1.28f, 1.0f},
        {0.74f, 0.74f, 1.0f},
	};

	for (std::size_t i = 0; i < kParticleCount; ++i) {
		particleBaseRotations_[i] = kBaseRotations[i];
		particleBaseScales_[i] = kBaseScales[i];
		hitParticleTransforms_[i] = {
		    .scale{kBaseScales[i]},
            .rotate{0.0f, 0.0f, kBaseRotations[i]},
            .translate{0.0f, 0.0f, 0.0f}
        };
		hitParticles_[i] = std::make_unique<Primitive>();
		hitParticles_[i]->Initialize(Primitive::Plane, kParticleTexturePath);
		hitParticles_[i]->SetTransform(hitParticleTransforms_[i]);
		hitParticles_[i]->SetEnableLighting(false);
		hitParticles_[i]->SetCamera(camera_);
	}

	enemyPosition_ = {0.0f, 0.0f, 0.0f};
}

void EnemyHitEffect::Activate(const Vector3& position) {
	isActive_ = true;
	activeTimer_ = activeDuration_;
	enemyPosition_ = position;
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

	const float progress = 1.0f - (activeTimer_ / activeDuration_);
	const float popScale = 0.75f + progress * 1.65f;
	const float fade = activeTimer_ / activeDuration_;

	hitTransform_.translate = enemyPosition_;
	hitTransform_.rotate.y = std::numbers::pi_v<float>;

	Matrix4x4 c = camera_->GetWorldMatrix();
	c.m[3][0] = c.m[3][1] = c.m[3][2] = 0;
	Matrix4x4 world = Function::Multiply(c, Function::MakeAffineMatrix(hitTransform_.scale, hitTransform_.rotate, hitTransform_.translate));

	hitEffect_->SetCamera(camera_);
	hitEffect_->SetWorldMatrix(world);
	hitEffect_->Update();

	for (std::size_t i = 0; i < kParticleCount; ++i) {
		hitParticleTransforms_[i].translate = enemyPosition_;
		hitParticleTransforms_[i].rotate.y = std::numbers::pi_v<float>;
		hitParticleTransforms_[i].rotate.z = particleBaseRotations_[i] + progress * (1.8f + 0.22f * static_cast<float>(i));
		hitParticleTransforms_[i].scale = {particleBaseScales_[i].x * popScale, particleBaseScales_[i].y * popScale, particleBaseScales_[i].z};

		Matrix4x4 particleWorld = Function::Multiply(c, Function::MakeAffineMatrix(hitParticleTransforms_[i].scale, hitParticleTransforms_[i].rotate, hitParticleTransforms_[i].translate));
		hitParticles_[i]->SetCamera(camera_);
		hitParticles_[i]->SetWorldMatrix(particleWorld);
		hitParticles_[i]->SetColor({1.0f, 0.72f, 0.24f, fade});
		hitParticles_[i]->Update();
	}
}

void EnemyHitEffect::Draw() {
	if (!isActive_) {
		return;
	}

	hitEffect_->Draw();
	for (auto& particle : hitParticles_) {
		particle->Draw();
	}
}
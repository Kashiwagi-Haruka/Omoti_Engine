#define NOMINMAX
#include "EnemyHitEffect.h"
#include "Function.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "ParticleManager.h"
#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <random>
#include <string>

namespace {
constexpr float kFrameTime = 1.0f / 60.0f;
constexpr const char* kParticleTexturePath = "Resources/2d/ArrowParticle.png";
constexpr float kIceShardMinSpeed = 4.0f;
constexpr float kIceShardMaxSpeed = 7.0f;
constexpr float kIceShardMinScale = 0.18f;
constexpr float kIceShardMaxScale = 0.32f;
constexpr float kIceShardSpawnRadius = 0.15f;
constexpr float kHitBillboardMinScale = 3.0f;
constexpr float kHitBillboardMaxScale = 5.85f;
constexpr const char* kParticleGroupPrefix = "enemyHitParticle_";
uint32_t gHitParticleGroupSerial = 0;
std::mt19937 gRandomEngine{std::random_device{}()};

float RandomRange(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(gRandomEngine);
}

Vector3 MakeRandomUnitVector() {
	const float azimuth = RandomRange(0.0f, std::numbers::pi_v<float> * 2.0f);
	const float y = RandomRange(-0.45f, 0.85f);
	const float horizontalLength = std::sqrt(1.0f - y * y);
	return {std::cos(azimuth) * horizontalLength, y, std::sin(azimuth) * horizontalLength};
}
} // namespace

void EnemyHitEffect::Initialize() {

	ModelManager::GetInstance()->LoadModel("Resources/3d", "HitEffect");
	ModelManager::GetInstance()->LoadModel("Resources/3d/HitIce", "HitIce");

	hitEffect_ = std::make_unique<Object3d>();
	hitEffect_->Initialize();
	hitEffect_->SetModel("HitEffect");
	hitEffect_->SetEnableLighting(false);
	hitEffect_->SetCamera(camera_);

	for (auto& billboard : hitBillboards_) {
		billboard.primitive = std::make_unique<Primitive>();
		billboard.primitive->SetEditorRegistrationEnabled(false);
		billboard.primitive->Initialize(Primitive::Plane, "Resources/2d/Effect/SizukuSwordEffect.png");
		billboard.primitive->SetEnableLighting(false);
		billboard.primitive->SetCamera(camera_);
		billboard.primitive->SetColor({0.45f, 0.75f, 1.0f, 0.85f});
		billboard.transform = {
		    .scale{1.0f, 1.0f, 1.0f},
            .rotate{0.0f, 0.0f, 0.0f},
            .translate{0.0f, 0.0f, 0.0f}
        };
		billboard.randomAngle = 0.0f;
		billboard.baseScale = 1.0f;
	}
	for (auto& shard : iceShards_) {
		shard.object = std::make_unique<Object3d>();
		shard.object->Initialize();
		shard.object->SetModel("HitIce");
		shard.object->SetEnableLighting(false);
		shard.object->SetCamera(camera_);
		shard.transform = {
		    .scale{0.25f, 0.25f, 0.25f},
            .rotate{0.0f,  0.0f,  0.0f },
            .translate{0.0f,  0.0f,  0.0f }
        };
		shard.velocity = {0.0f, 0.0f, 0.0f};
		shard.angularVelocity = {0.0f, 0.0f, 0.0f};
		shard.baseScale = 0.25f;
	}

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
void EnemyHitEffect::SetCamera(Camera* camera) {
	camera_ = camera;
	RefreshCameraMatrices();
}
void EnemyHitEffect::Activate(const Vector3& position) {
	isActive_ = true;
	activeTimer_ = activeDuration_;
	enemyPosition_ = position;

	for (auto& billboard : hitBillboards_) {
		const float scale = RandomRange(kHitBillboardMinScale, kHitBillboardMaxScale);
		billboard.baseScale = scale+enemyScale_.x;
		billboard.randomAngle = RandomRange(0.0f, std::numbers::pi_v<float> * 2.0f);
		billboard.transform.scale = {scale+enemyScale_.x, scale+enemyScale_.y, scale+enemyScale_.z};
		billboard.transform.rotate = {0.0f, 0.0f, billboard.randomAngle};
		billboard.transform.translate = enemyPosition_;
		if (billboard.primitive) {
			billboard.primitive->SetCamera(camera_);
			billboard.primitive->SetColor({0.45f, 0.75f, 1.0f, 0.9f});
		}
	}

	for (auto& shard : iceShards_) {
		const Vector3 direction = MakeRandomUnitVector();
		const float speed = RandomRange(kIceShardMinSpeed, kIceShardMaxSpeed);
		const float scale = RandomRange(kIceShardMinScale, kIceShardMaxScale);
		shard.baseScale = scale+enemyScale_.x;
		shard.transform.scale = {scale+enemyScale_.x, scale+enemyScale_.y, scale+enemyScale_.z};
		shard.transform.rotate = {RandomRange(0.0f, std::numbers::pi_v<float> * 2.0f), RandomRange(0.0f, std::numbers::pi_v<float> * 2.0f), RandomRange(0.0f, std::numbers::pi_v<float> * 2.0f)};
		shard.transform.translate = enemyPosition_ + direction * kIceShardSpawnRadius;
		shard.velocity = direction * speed;
		shard.angularVelocity = {RandomRange(-9.0f, 9.0f), RandomRange(-9.0f, 9.0f), RandomRange(-9.0f, 9.0f)};
		if (shard.object) {
			shard.object->SetCamera(camera_);
			shard.object->SetTransform(shard.transform);
		}
	}

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

	const float deltaTime = kFrameTime;
	const float scaleRatio = std::max(activeTimer_ / activeDuration_, 0.0f);
	Matrix4x4 billboardMatrix = Function::MakeIdentity4x4();
	if (camera_) {
		billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
		billboardMatrix.m[3][0] = billboardMatrix.m[3][1] = billboardMatrix.m[3][2] = 0.0f;
	}
	for (auto& billboard : hitBillboards_) {
		const float currentScale = billboard.baseScale * scaleRatio;
		billboard.transform.scale = {currentScale, currentScale, currentScale};
		billboard.transform.translate = enemyPosition_;
		const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(billboard.transform.scale);
		const Matrix4x4 angleMatrix = Function::MakeRotateZMatrix(billboard.randomAngle);
		const Matrix4x4 translateMatrix = Function::MakeTranslateMatrix(billboard.transform.translate);
		const Matrix4x4 worldMatrix = Function::Multiply(Function::Multiply(scaleMatrix, Function::Multiply(angleMatrix, billboardMatrix)), translateMatrix);
		if (billboard.primitive) {
			billboard.primitive->SetCamera(camera_);
			billboard.primitive->SetColor({0.45f, 0.75f, 1.0f, 0.9f * scaleRatio});
			billboard.primitive->SetWorldMatrix(worldMatrix);
			billboard.primitive->UpdateCameraMatrices();
		}
	}
	for (auto& shard : iceShards_) {
		shard.transform.translate += shard.velocity * deltaTime;
		shard.transform.rotate += shard.angularVelocity * deltaTime;
		const float currentScale = shard.baseScale * scaleRatio;
		shard.transform.scale = {currentScale, currentScale, currentScale};
		if (shard.object) {
			shard.object->SetCamera(camera_);
			shard.object->SetTransform(shard.transform);
			shard.object->Update();
		}
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
void EnemyHitEffect::RefreshCameraMatrices() {
	if (!isActive_ || !camera_) {
		return;
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = billboardMatrix.m[3][1] = billboardMatrix.m[3][2] = 0.0f;
	for (auto& billboard : hitBillboards_) {
		const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(billboard.transform.scale);
		const Matrix4x4 angleMatrix = Function::MakeRotateZMatrix(billboard.randomAngle);
		const Matrix4x4 translateMatrix = Function::MakeTranslateMatrix(billboard.transform.translate);
		const Matrix4x4 worldMatrix = Function::Multiply(Function::Multiply(scaleMatrix, Function::Multiply(angleMatrix, billboardMatrix)), translateMatrix);
		if (billboard.primitive) {
			billboard.primitive->SetCamera(camera_);
			billboard.primitive->SetWorldMatrix(worldMatrix);
			billboard.primitive->UpdateCameraMatrices();
		}
	}
	for (auto& shard : iceShards_) {
		if (shard.object) {
			shard.object->SetCamera(camera_);
			shard.object->UpdateCameraMatrices();
		}
	}

	Matrix4x4 cameraWorldMatrix = camera_->GetWorldMatrix();
	cameraWorldMatrix.m[3][0] = cameraWorldMatrix.m[3][1] = cameraWorldMatrix.m[3][2] = 0.0f;
	const Matrix4x4 worldMatrix = Function::Multiply(cameraWorldMatrix, Function::MakeAffineMatrix(hitTransform_.scale, hitTransform_.rotate, hitTransform_.translate));
	ParticleManager::GetInstance()->SetCamera(camera_);
	hitEffect_->SetCamera(camera_);
	hitEffect_->SetWorldMatrix(worldMatrix);
	hitEffect_->UpdateCameraMatrices();
}

void EnemyHitEffect::Draw() {
	if (!isActive_) {
		return;
	}

	hitEffect_->Draw();
	Object3dCommon::GetInstance()->DrawCommon();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	for (auto& billboard : hitBillboards_) {
		if (billboard.primitive) {
			billboard.primitive->Draw();
		}
	}
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	for (auto& shard : iceShards_) {
		if (shard.object) {
			shard.object->Draw();
		}
	}
	if (hitParticleEmitter_) {
		hitParticleEmitter_->Draw();
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
}
#define NOMINMAX
#include "SizukuSpecialCamera.h"
#include "Function.h"
#include "GameBase.h"
#include <algorithm>
#include <cmath>

void SizukuSpecialCamera::Initialize() {
	camera_ = std::make_unique<Camera>();
	transform_ = {
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
	camera_->SetTransform(transform_);
	camera_->Update();
	cameraCut_ = CameraCut::TURN;
}
void SizukuSpecialCamera::Start() {
	isEnd_ = false;
	cameraCut_ = CameraCut::TURN;
	animationTime_ = 0.0f;
}

void SizukuSpecialCamera::Update() {
	if (!isEnd_) {
		const float deltaTime = GameBase::GetInstance()->GetDeltaTime();
		switch (attribute_) {
		case Attribute::Fire:
			UpdateFireCamera(deltaTime);
			break;
		case Attribute::Wind:
			UpdateWindCamera(deltaTime);
			break;
		case Attribute::Thunder:
			UpdateThunderCamera(deltaTime);
			break;
		case Attribute::Imaginary:
			UpdateImaginaryCamera(deltaTime);
			break;
		case Attribute::Quantum:
			UpdateQuantumCamera(deltaTime);
			break;
		case Attribute::Ice:
		case Attribute::None:
		case Attribute::MAXATTRIBUTE:
		default:
			UpdateIceCamera(deltaTime);
			break;
		}
	}
	camera_->SetTransform(transform_);
	camera_->Update();
}

void SizukuSpecialCamera::LookAt(const Vector3& target) {
	const Vector3 direction = Function::Normalize(target - transform_.translate);
	transform_.rotate.x = std::asin(-direction.y);
	transform_.rotate.y = std::atan2(direction.x, direction.z);
	transform_.rotate.z = 0.0f;
}

void SizukuSpecialCamera::UpdateIceCamera(float deltaTime) {
	const float playerYaw = playerTransform_.rotate.y;
	const Vector3 playerForward = {std::sin(playerYaw), 0.0f, std::cos(playerYaw)};
	const Vector3 playerRight = {std::cos(playerYaw), 0.0f, -std::sin(playerYaw)};
	const Vector3 lookAt = playerTransform_.translate + Vector3{0.0f, lookAtHeight_, 0.0f};

	switch (cameraCut_) {
	case SizukuSpecialCamera::CameraCut::TURN:
		animationTime_ += deltaTime / TurnAnimationTime_;
		if (animationTime_ >= 1.0f) {
			animationTime_ = 0.0f;
			cameraCut_ = CameraCut::FINGERSNAP;
		}
		distance_ = 3.0f;
		transform_.translate = playerTransform_.translate + playerForward * distance_ + playerRight * 0.75f;
		transform_.translate.y += cameraHeight_ + 0.8f;

		LookAt(lookAt);
		break;
	case SizukuSpecialCamera::CameraCut::FINGERSNAP:

		animationTime_ += deltaTime / FingerSnapAnimationTime_;
		if (animationTime_ >= 1.0f) {
			animationTime_ = 0.0f;
			cameraCut_ = CameraCut::ATTACK;
		}
		distance_ = 3.0f;
		transform_.translate = playerTransform_.translate + playerForward * distance_;
		transform_.translate.y += cameraHeight_;

		LookAt(lookAt);
		break;
	case SizukuSpecialCamera::CameraCut::ATTACK:
		animationTime_ += deltaTime / AttackAnimationTime_;
		if (animationTime_ >= 1.0f) {
			animationTime_ = 0.0f;
			isEnd_ = true;
		}
		distance_ = 5.0f;
		transform_.translate = playerTransform_.translate - playerForward * distance_ - playerRight * 0.75f;
		transform_.translate.y += cameraHeight_ - 0.6f;

		LookAt(lookAt);
		break;
	default:
		break;
	}
}

void SizukuSpecialCamera::UpdateFireCamera(float deltaTime) {
	// 花火を見上げるため、背後から上空へチルトする。
	animationTime_ += deltaTime;
	const float yaw = playerTransform_.rotate.y;
	const Vector3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
	transform_.translate = playerTransform_.translate - forward * 7.0f + Vector3{0.0f, 3.5f, 0.0f};
	LookAt(playerTransform_.translate + Vector3{0.0f, 8.0f, 0.0f});
	isEnd_ = animationTime_ >= 3.4f;
}

void SizukuSpecialCamera::UpdateWindCamera(float deltaTime) {
	// 竜巻に合わせてプレイヤーの周囲を一周する。
	animationTime_ += deltaTime;
	const float angle = playerTransform_.rotate.y + animationTime_ * 1.8f;
	transform_.translate = playerTransform_.translate + Vector3{std::sin(angle) * 7.0f, 3.2f, std::cos(angle) * 7.0f};
	LookAt(playerTransform_.translate + Vector3{0.0f, 2.0f, 0.0f});
	isEnd_ = animationTime_ >= 4.0f;
}

void SizukuSpecialCamera::UpdateThunderCamera(float deltaTime) {
	// 発射方向を見せる低い横位置へ素早く切り替える。
	animationTime_ += deltaTime;
	const float yaw = playerTransform_.rotate.y;
	const Vector3 forward = {std::sin(yaw), 0.0f, std::cos(yaw)};
	const Vector3 right = {std::cos(yaw), 0.0f, -std::sin(yaw)};
	transform_.translate = playerTransform_.translate - forward * 3.0f + right * 4.0f + Vector3{0.0f, 1.0f, 0.0f};
	LookAt(playerTransform_.translate + forward * 12.0f + Vector3{0.0f, 1.0f, 0.0f});
	isEnd_ = animationTime_ >= 2.0f;
}

void SizukuSpecialCamera::UpdateImaginaryCamera(float deltaTime) {
	// 光の解放を俯瞰できるよう、ゆっくり上昇する。
	animationTime_ += deltaTime;
	const float height = 4.0f + std::min(animationTime_ * 2.0f, 5.0f);
	transform_.translate = playerTransform_.translate + Vector3{0.0f, height, -8.0f};
	LookAt(playerTransform_.translate + Vector3{0.0f, 1.0f, 0.0f});
	isEnd_ = animationTime_ >= 3.6f;
}

void SizukuSpecialCamera::UpdateQuantumCamera(float deltaTime) {
	// 二つの破裂位置を斜め上から同時に画面へ収める。
	animationTime_ += deltaTime;
	const float side = animationTime_ < 1.1f ? -1.0f : 1.0f;
	transform_.translate = playerTransform_.translate + Vector3{side * 7.0f, 5.0f, -7.0f};
	LookAt(playerTransform_.translate + Vector3{0.0f, 1.5f, 0.0f});
	isEnd_ = animationTime_ >= 3.7f;
}
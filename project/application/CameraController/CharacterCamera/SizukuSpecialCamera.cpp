#include "SizukuSpecialCamera.h"
#include "Function.h"
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
}

void SizukuSpecialCamera::Update() {
	// プレイヤーの向いている方向へカメラを置き、胸元を注視する。
	const float playerYaw = playerTransform_.rotate.y;
	const Vector3 playerForward = {std::sin(playerYaw), 0.0f, std::cos(playerYaw)};
	transform_.translate = playerTransform_.translate + playerForward * distance_;
	transform_.translate.y += cameraHeight_;

	const Vector3 lookAt = playerTransform_.translate + Vector3{0.0f, lookAtHeight_, 0.0f};
	Vector3 direction = Function::Normalize(lookAt - transform_.translate);
	transform_.rotate.x = std::asin(-direction.y);
	transform_.rotate.y = std::atan2(direction.x, direction.z);
	transform_.rotate.z = 0.0f;

	camera_->SetTransform(transform_);
	camera_->Update();
}
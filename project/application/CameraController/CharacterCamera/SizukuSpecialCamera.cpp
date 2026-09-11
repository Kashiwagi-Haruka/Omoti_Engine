#include "SizukuSpecialCamera.h"
#include "Function.h"
#include "GameBase.h"
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
		const float playerYaw = playerTransform_.rotate.y;
		const Vector3 playerForward = {std::sin(playerYaw), 0.0f, std::cos(playerYaw)};
		const Vector3 playerRight = {std::cos(playerYaw), 0.0f, -std::sin(playerYaw)};
		const Vector3 lookAt = playerTransform_.translate + Vector3{0.0f, lookAtHeight_, 0.0f};
		Vector3 direction{};

		switch (cameraCut_) {
		case SizukuSpecialCamera::CameraCut::TURN:
			animationTime_ += GameBase::GetInstance()->GetDeltaTime() / TurnAnimationTime_;
			if (animationTime_ >= 1.0f) {
				animationTime_ = 0.0f;
				cameraCut_ = CameraCut::FINGERSNAP;
			}
			distance_ = 3.0f;
			transform_.translate = playerTransform_.translate + playerForward * distance_ + playerRight * 0.75f;
			transform_.translate.y += cameraHeight_ + 0.8f;

			direction = Function::Normalize(lookAt - transform_.translate);
			transform_.rotate.x = std::asin(-direction.y);
			transform_.rotate.y = std::atan2(direction.x, direction.z);
			transform_.rotate.z = 0.0f;
			break;
		case SizukuSpecialCamera::CameraCut::FINGERSNAP:

			animationTime_ += GameBase::GetInstance()->GetDeltaTime() / FingerSnapAnimationTime_;
			if (animationTime_ >= 1.0f) {
				animationTime_ = 0.0f;
				cameraCut_ = CameraCut::ATTACK;
			}
			distance_ = 3.0f;
			transform_.translate = playerTransform_.translate + playerForward * distance_;
			transform_.translate.y += cameraHeight_;

			direction = Function::Normalize(lookAt - transform_.translate);
			transform_.rotate.x = std::asin(-direction.y);
			transform_.rotate.y = std::atan2(direction.x, direction.z);
			transform_.rotate.z = 0.0f;
			break;
		case SizukuSpecialCamera::CameraCut::ATTACK:
			animationTime_ += GameBase::GetInstance()->GetDeltaTime() / AttackAnimationTime_;
			if (animationTime_ >= 1.0f) {
				animationTime_ = 0.0f;
				isEnd_ = true;
			}
			distance_ = 5.0f;
			transform_.translate = playerTransform_.translate - playerForward * distance_ - playerRight * 0.75f;
			transform_.translate.y += cameraHeight_ - 0.6f;

			direction = Function::Normalize(lookAt - transform_.translate);
			transform_.rotate.x = std::asin(-direction.y);
			transform_.rotate.y = std::atan2(direction.x, direction.z);
			transform_.rotate.z = 0.0f;
			break;
		default:
			break;
		}
	}
	camera_->SetTransform(transform_);
	camera_->Update();
}
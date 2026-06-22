#include "CameraController.h"
#include "Camera.h"
#include "Input.h"
#include "Function.h"
#include <algorithm>
#include <numbers>

namespace {

float NormalizeAngle(float angle) {
	const float twoPi = 2.0f * std::numbers::pi_v<float>;
	while (angle > std::numbers::pi_v<float>) {
		angle -= twoPi;
	}
	while (angle < -std::numbers::pi_v<float>) {
		angle += twoPi;
	}
	return angle;
}

Vector3 LerpRotationShortest(const Vector3& start, const Vector3& end, float ratio) {
	return {
	    Function::Lerp(start.x, start.x + NormalizeAngle(end.x - start.x), ratio),
	    Function::Lerp(start.y, start.y + NormalizeAngle(end.y - start.y), ratio),
	    Function::Lerp(start.z, start.z + NormalizeAngle(end.z - start.z), ratio),
	};
}

} // namespace
void CameraController::Initialize() {
	playerCamera_ = std::make_unique<PlayerCamera>();
	playerCamera_->Initialize();

	lockOnCamera_ = std::make_unique<LockOnCamera>();
	lockOnCamera_->Initialize();

	blendCamera_ = std::make_unique<Camera>();
	switchStartTransform_ = playerCamera_->GetTransform();
	blendCamera_->SetTransform(switchStartTransform_);
	blendCamera_->Update();
	camera_ = blendCamera_.get();
}

void CameraController::Update() {
	if (autoLockOnTimer_ > 0.0f) {
		autoLockOnTimer_ -= 1.0f / 60.0f;
		cameraMode_ = CameraMode::kLockOnCamera;
		if (autoLockOnTimer_ <= 0.0f) {
			autoLockOnTimer_ = 0.0f;
			lockOnCamera_->ClearTarget();
			cameraMode_ = CameraMode::kPlayerCamera;
		}
	}

	playerCamera_->SetPlayerPos(playerPos);
	playerCamera_->Update();
	const Vector2 mouseMove = Input::GetInstance()->GetMouseMove();
	const bool isMouseMoved = mouseMove.x != 0.0f || mouseMove.y != 0.0f;
	if (isMouseMoved && cameraMode_ != CameraMode::kPlayerCamera) {
		cameraMode_ = CameraMode::kPlayerCamera;
		autoLockOnTimer_ = 0.0f;
		isCameraSwitching_ = false;
		blendCamera_->SetTransform(playerCamera_->GetTransform());
	}
	lockOnCamera_->SetPlayerPos(playerPos);
	lockOnCamera_->SetFollowPosition(playerCamera_->GetTransform().translate);
	lockOnCamera_->SetOrbitPitch(playerCamera_->GetTransform().rotate.x);
	lockOnCamera_->Update();

	const Transform targetTransform = (cameraMode_ == CameraMode::kLockOnCamera) ? lockOnCamera_->GetTransform() : playerCamera_->GetTransform();

	if (preCameraMode_ != cameraMode_) {
		isCameraSwitching_ = true;
		cameraSwitchTimer_ = 0.0f;
		switchStartTransform_ = blendCamera_->GetTransform();
	}

	if (isCameraSwitching_) {
		cameraSwitchTimer_ += 1.0f / 60.0f;
		const float t = std::clamp(cameraSwitchTimer_ / cameraSwitchDuration_, 0.0f, 1.0f);
		Transform blendedTransform = targetTransform;
		blendedTransform.translate = Function::Lerp(switchStartTransform_.translate, targetTransform.translate, t);
		blendedTransform.rotate = LerpRotationShortest(switchStartTransform_.rotate, targetTransform.rotate, t);
		blendCamera_->SetTransform(blendedTransform);
		if (t >= 1.0f) {
			isCameraSwitching_ = false;
		}
	} else {
		blendCamera_->SetTransform(targetTransform);
	}

	blendCamera_->Update();
	camera_ = blendCamera_.get();
	preCameraMode_ = cameraMode_;
}

Camera* CameraController::GetCamera() { return camera_; }
void CameraController::StartShake(float durationSeconds) { playerCamera_->StartShake(durationSeconds); }
void CameraController::SetLockOnTarget(const Vector3& targetPos, float durationSeconds) {
	lockOnCamera_->SetTargetPos(targetPos);
	autoLockOnTimer_ = durationSeconds;
	cameraMode_ = CameraMode::kLockOnCamera;
}
#include "CameraController.h"
#include "Camera.h"
#include "Input.h"
#include "Function.h"
#include "PlayCommand/PlayCommand.h"
#include <algorithm>
#include <numbers>
#include "ImGui.h"

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
float SmoothStep(float ratio) { return ratio * ratio * (3.0f - 2.0f * ratio); }
} // namespace
void CameraController::Initialize() {
	playerCamera_ = std::make_unique<PlayerCamera>();
	playerCamera_->Initialize();

	lockOnCamera_ = std::make_unique<LockOnCamera>();
	lockOnCamera_->Initialize();

	normalAttackCamera_ = std::make_unique<NormalAttackCamera>();
	normalAttackCamera_->Initialize();

	blendCamera_ = std::make_unique<Camera>();
	switchStartTransform_ = playerCamera_->GetTransform();
	blendCamera_->SetTransform(switchStartTransform_);
	blendCamera_->Update();
	camera_ = blendCamera_.get();
}

void CameraController::Update() {
	constexpr float kDeltaTime = 1.0f / 60.0f;
	constexpr float kNormalAttackCameraReturnDelay = 0.8f;

	if (PlayCommand::GetNORMAL_ATTACK_PUSH()) {
		normalAttackIdleTimer_ = 0.0f;
	} else {
		normalAttackIdleTimer_ += kDeltaTime;
	}

	if (autoLockOnTimer_ > 0.0f) {
		autoLockOnTimer_ -= kDeltaTime;
		cameraMode_ = CameraMode::kLockOnCamera;
		if (autoLockOnTimer_ <= 0.0f) {
			ReturnToPlayerCamera();
		}
	}

	if (cameraMode_ == CameraMode::kNormalAttackCamera && normalAttackIdleTimer_ >= kNormalAttackCameraReturnDelay) {
		cameraMode_ = CameraMode::kPlayerCamera;
		autoLockOnTimer_ = 0.0f;
		lockOnCamera_->ClearTarget();
		normalAttackCamera_->ClearTarget();
	}

	const Vector2 mouseMove = Input::GetInstance()->GetMouseMove();
	const Vector2 rightStick = Input::GetInstance()->GetJoyStickRXY();
	const bool isCameraMoved = mouseMove.x != 0.0f || mouseMove.y != 0.0f || rightStick.x != 0.0f || rightStick.y != 0.0f;
	const bool canCameraInputReturnToPlayerCamera = cameraMode_ != CameraMode::kPlayerCamera && cameraMode_ != CameraMode::kNormalAttackCamera;
	if (isCameraMoved && canCameraInputReturnToPlayerCamera) {
		InheritPlayerCameraRotation(cameraMode_);
		cameraMode_ = CameraMode::kPlayerCamera;
		autoLockOnTimer_ = 0.0f;
	}
	if (preCameraMode_ != cameraMode_ && cameraMode_ == CameraMode::kPlayerCamera) {
		InheritPlayerCameraRotation(preCameraMode_);
	}

	playerCamera_->SetPlayerPos(playerPos);
	playerCamera_->Update();
	lockOnCamera_->SetPlayerPos(playerPos);
	lockOnCamera_->SetFollowPosition(playerCamera_->GetTransform().translate);
	lockOnCamera_->SetOrbitPitch(playerCamera_->GetTransform().rotate.x);
	lockOnCamera_->Update();

	normalAttackCamera_->SetPlayerPos(playerPos);
	normalAttackCamera_->SetFollowPosition(playerCamera_->GetTransform().translate);
	normalAttackCamera_->SetOrbitPitch(playerCamera_->GetTransform().rotate.x);
	normalAttackCamera_->Update();

	Transform targetTransform = playerCamera_->GetTransform();
	if (cameraMode_ == CameraMode::kLockOnCamera) {
		targetTransform = lockOnCamera_->GetTransform();
	} else if (cameraMode_ == CameraMode::kNormalAttackCamera) {
		targetTransform = normalAttackCamera_->GetTransform();
	}

	if (preCameraMode_ != cameraMode_) {
		isCameraSwitching_ = true;
		cameraSwitchTimer_ = 0.0f;
		switchStartTransform_ = blendCamera_->GetTransform();
		cameraSwitchDuration_ = (preCameraMode_ == CameraMode::kNormalAttackCamera && cameraMode_ == CameraMode::kLockOnCamera) ? kNormalAttackToLockOnSwitchDuration_ : kDefaultCameraSwitchDuration_;
	}

	if (isCameraSwitching_) {
		cameraSwitchTimer_ += kDeltaTime;
		const float linearT = std::clamp(cameraSwitchTimer_ / cameraSwitchDuration_, 0.0f, 1.0f);
		const float t = SmoothStep(linearT);
		Transform blendedTransform = targetTransform;
		blendedTransform.translate = Function::Lerp(switchStartTransform_.translate, targetTransform.translate, t);
		blendedTransform.rotate = LerpRotationShortest(switchStartTransform_.rotate, targetTransform.rotate, t);
		blendCamera_->SetTransform(blendedTransform);
		if (linearT >= 1.0f) {
			isCameraSwitching_ = false;
		}
	} else {
		blendCamera_->SetTransform(targetTransform);
	}

	blendCamera_->Update();
	camera_ = blendCamera_.get();
#ifdef USE_IMGUI
	
	if(ImGui::Begin("CameraMode")){
		ImGui::Text("Now");
		switch (cameraMode_) {
		case CameraController::CameraMode::kPlayerCamera:
			ImGui::Text("playerCamera");
			break;
		case CameraController::CameraMode::kLockOnCamera:
			ImGui::Text("kLockCamera");
			break;
		case CameraController::CameraMode::kNormalAttackCamera:
			ImGui::Text("AttackCamera");
			break;
		default:
			break;
		}
		ImGui::Text("Pre");
		switch (preCameraMode_) {
		case CameraController::CameraMode::kPlayerCamera:
			ImGui::Text("playerCamera");
			break;
		case CameraController::CameraMode::kLockOnCamera:
			ImGui::Text("kLockCamera");
			break;
		case CameraController::CameraMode::kNormalAttackCamera:
			ImGui::Text("AttackCamera");
			break;
		default:
			break;
		}
		
	}
	ImGui::End();

#endif // USE_IMGUI

	preCameraMode_ = cameraMode_;
}

void CameraController::InheritPlayerCameraRotation(CameraMode sourceMode) {
	if (sourceMode == CameraMode::kLockOnCamera) {
		playerCamera_->SetOrbitRotation(lockOnCamera_->GetTransform().rotate);
	} else if (sourceMode == CameraMode::kNormalAttackCamera) {
		playerCamera_->SetOrbitRotation(normalAttackCamera_->GetTransform().rotate);
	}
}

void CameraController::ReturnToPlayerCamera() {
	if (cameraMode_ != CameraMode::kPlayerCamera) {
		InheritPlayerCameraRotation(cameraMode_);
	}
	cameraMode_ = CameraMode::kPlayerCamera;
	autoLockOnTimer_ = 0.0f;
	normalAttackIdleTimer_ = 1.0f;
	lockOnCamera_->ClearTarget();
	normalAttackCamera_->ClearTarget();
}

Camera* CameraController::GetCamera() { return camera_; }
void CameraController::StartShake(float durationSeconds) { playerCamera_->StartShake(durationSeconds); }
void CameraController::LookAtFromPlayerPosition(const Vector3& targetPos) {
	ReturnToPlayerCamera();
	playerCamera_->LookAtFromPlayerPosition(targetPos);
}

void CameraController::SetLockOnTarget(const Vector3& targetPos, float durationSeconds) {
	lockOnCamera_->SetTargetPos(targetPos);
	normalAttackIdleTimer_ = 0.0f;
	autoLockOnTimer_ = durationSeconds;
	cameraMode_ = CameraMode::kLockOnCamera;
}
void CameraController::SetNormalAttackTarget(const Vector3& targetPos) {
	normalAttackCamera_->SetTargetPos(targetPos);
	normalAttackIdleTimer_ = 0.0f;
	autoLockOnTimer_ = 0.0f;
	lockOnCamera_->ClearTarget();
	cameraMode_ = CameraMode::kNormalAttackCamera;
}
void CameraController::ClearAttackCameraTarget() { ReturnToPlayerCamera(); }
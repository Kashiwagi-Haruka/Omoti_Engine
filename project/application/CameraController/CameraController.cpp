#include "CameraController.h"

void CameraController::Initialize() {
	playerCamera_ = std::make_unique<PlayerCamera>();
	playerCamera_->Initialize();
	camera_ = playerCamera_->GetCamera();
}
void CameraController::Update() { 
	switch (cameraMode_) {
	case CameraController::CameraMode::kPlayerCamera:
		playerCamera_->Update(); 
		camera_ = playerCamera_->GetCamera();
		break;
	case CameraController::CameraMode::kLockOnCamera:
		break;
	case CameraController::CameraMode::kNormalAttackCamera:
		break;
	default:
		break;
	}
}
Camera* CameraController::GetCamera() { return camera_; }
void CameraController::StartShake(float durationSeconds) { playerCamera_->StartShake(durationSeconds); }
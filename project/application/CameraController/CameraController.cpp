#include "CameraController.h"
#include "Camera.h"


#include "Function.h"
void CameraController::Initialize() {
	playerCamera_ = std::make_unique<PlayerCamera>();
	playerCamera_->Initialize();

	lockOnCamera_ = std::make_unique<LockOnCamera>();
	lockOnCamera_->Initialize();
	camera_ = playerCamera_->GetCamera();
}	
void CameraController::Update() { 
	switch (cameraMode_) {
	case CameraController::CameraMode::kPlayerCamera:
		playerCamera_->SetPlayerPos(playerPos);
		playerCamera_->Update();
		if (preCameraMode_ != CameraMode::kPlayerCamera) {
			isCameraSwitching_ = true;
			if (cameraMode_ == CameraMode::kLockOnCamera) {
				camera_->SetTranslate(Function::Lerp(playerCamera_->GetTransform().translate, lockOnCamera_->GetTransform().translate, cameraSwitchTimer_));	
			} else {
				
			}
			
		} else {
			camera_ = playerCamera_->GetCamera();
		}
		
		break;
	case CameraController::CameraMode::kLockOnCamera:
		lockOnCamera_->SetPlayerPos(playerPos);
		lockOnCamera_->Update();
		camera_ = lockOnCamera_->GetCamera();
		break;
	case CameraController::CameraMode::kNormalAttackCamera:
		break;
	default:
		break;
	}
}
Camera* CameraController::GetCamera() { return camera_; }
void CameraController::StartShake(float durationSeconds) { playerCamera_->StartShake(durationSeconds); }
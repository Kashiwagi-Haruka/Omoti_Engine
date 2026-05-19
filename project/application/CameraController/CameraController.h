#pragma once
#include <memory>
#include "Vector3.h"
#include "Transform.h"
#include "LockOnCamera/LockOnCamera.h"
#include "PlayerCamera/PlayerCamera.h"

class Camera;

class CameraController {

	enum class CameraMode {
		kPlayerCamera,
		kLockOnCamera,
		kNormalAttackCamera 
	};
	CameraMode cameraMode_ = CameraMode::kPlayerCamera;
	CameraMode preCameraMode_ = CameraMode::kPlayerCamera;
	std::unique_ptr<PlayerCamera> playerCamera_;
	std::unique_ptr<LockOnCamera> lockOnCamera_;

	Vector3 playerPos = {0.0f, 0.0f, 0.0f};
	Transform transform_{};
	Camera* camera_ = nullptr;

	bool isCameraSwitching_ = false;
	float cameraSwitchTimer_ = 0.0f;

public:
	void Initialize();
	void Update();
	Camera* GetCamera();
	Transform GetTransform() { return transform_; }
	void SetTransform(Transform transform) { transform_ = transform; }
	void SetPlayerPos(const Vector3& pos) { playerPos = pos; }
	void StartShake(float durationSeconds = 1.0f);
	void SetCameraMode(CameraMode mode) { cameraMode_ = mode; }
};
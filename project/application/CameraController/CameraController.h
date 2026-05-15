#pragma once
#include <memory>

class Camera;
class PlayerCamera;
class LockOnCamera;
class CameraController {

	enum class CameraMode {
		kPlayerCamera,
		kLockOnCamera,
		kNormalAttackCamera 
	};
	CameraMode cameraMode_ = CameraMode::kPlayerCamera;
	std::unique_ptr<PlayerCamera> playerCamera_;
	std::unique_ptr<LockOnCamera> lockOnCamera_;

	Vector3 playerPos = {0.0f, 0.0f, 0.0f};
	Transform transform_{};
	Camera* camera_ = nullptr;

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
#pragma once
#include "Transform.h"
#include <memory>

class Camera;

class LockOnCamera {

	Transform transform_{};
	std::unique_ptr<Camera> camera_;
	float mouseSensitivity_ = 0.002f;
	Vector3 playerPos = {0.0f, 0.0f, 0.0f};
	float orbitYaw_ = 0.0f;
	float orbitPitch_ = 0.15f;
	float shakeTimer_ = 0.0f;
	float shakeDuration_ = 0.0f;
	float shakeAmplitude_ = 0.6f;
	bool hasTarget_ = false;
	Vector3 targetPos_ = {0.0f, 0.0f, 0.0f};
	bool hasFollowPosition_ = false;
	Vector3 followPosition_ = {0.0f, 0.0f, 0.0f};
	float distance_ = 5.0f;

public:
	LockOnCamera();
	~LockOnCamera();
	void Initialize();
	void Update();
	void StartShake(float durationSeconds = 1.0f);

	Camera* GetCamera();
	Transform GetTransform() { return transform_; }
	void SetTransform(Transform transform) { transform_ = transform; }
	void SetPlayerPos(const Vector3& pos) { playerPos = pos; }
	void SetOrbitPitch(float pitch) { orbitPitch_ = pitch; }
	void SetTargetPos(const Vector3& pos) {
		targetPos_ = pos;
		hasTarget_ = true;
	}
	void ClearTarget() { hasTarget_ = false; }
	void SetFollowPosition(const Vector3& pos) {
		followPosition_ = pos;
		hasFollowPosition_ = true;
	}
	void ClearFollowPosition() { hasFollowPosition_ = false; }
	void SetDistance(float distance) { distance_ = distance; }
};
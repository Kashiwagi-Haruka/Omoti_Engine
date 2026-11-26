#pragma once

#include "Camera.h"
#include "Function.h"
#include "GameBase.h"

#include <cmath>
#include <memory>
#include <random>

class TargetOrbitCamera {
public:
	virtual ~TargetOrbitCamera() = default;

	void Initialize() {
		transform_.scale = {1.0f, 1.0f, 1.0f};
		transform_.rotate = {orbitPitch_, orbitYaw_, 0.0f};
		transform_.translate = {0.0f, 10.0f, -50.0f};

		camera_ = std::make_unique<Camera>();
		camera_->SetTransform(transform_);
	}

	void Update() {
		if (hasTarget_) {
			UpdateYaw(targetPosition_ - playerPosition_);
		} else if (hasFollowPosition_) {
			UpdateYaw(playerPosition_ - followPosition_);
		}

		constexpr float kMaxPitch = 1.2f;
		if (orbitPitch_ > kMaxPitch) {
			orbitPitch_ = kMaxPitch;
		} else if (orbitPitch_ < -kMaxPitch) {
			orbitPitch_ = -kMaxPitch;
		}

		const float cosPitch = std::cos(orbitPitch_);
		const Vector3 direction = {
		    std::sin(orbitYaw_) * cosPitch,
		    -std::sin(orbitPitch_),
		    std::cos(orbitYaw_) * cosPitch,
		};

		transform_.translate = playerPosition_ - direction * distance_;
		transform_.rotate = {orbitPitch_, orbitYaw_, 0.0f};

		ApplyShake();

		camera_->SetTransform(transform_);
		camera_->Update();
	}

	Camera* GetCamera() { return camera_.get(); }
	Transform GetTransform() { return transform_; }
	void SetTransform(Transform transform) { transform_ = transform; }
	void SetPlayerPos(const Vector3& pos) { playerPosition_ = pos; }
	void SetOrbitPitch(float pitch) { orbitPitch_ = pitch; }
	void SetDistance(float distance) { distance_ = distance; }

	void SetTargetPos(const Vector3& pos) {
		targetPosition_ = pos;
		hasTarget_ = true;
	}

	void ClearTarget() { hasTarget_ = false; }

	void SetFollowPosition(const Vector3& pos) {
		followPosition_ = pos;
		hasFollowPosition_ = true;
	}

	void ClearFollowPosition() { hasFollowPosition_ = false; }

	void StartShake(float durationSeconds = 1.0f) {
		shakeDuration_ = durationSeconds;
		shakeTimer_ = durationSeconds;
	}

protected:
	TargetOrbitCamera() = default;

private:
	void UpdateYaw(const Vector3& direction) {
		constexpr float kMinDirectionLengthSquared = 0.0001f;
		if (Function::LengthSquared(direction) > kMinDirectionLengthSquared) {
			const float targetYaw = std::atan2(direction.x, direction.z);
			orbitYaw_ = Function::GetNearestAngle(orbitYaw_, targetYaw);
		}
	}

	void ApplyShake() {
		if (shakeTimer_ <= 0.0f) {
			return;
		}

		shakeTimer_ -= GameBase::GetInstance()->GetDeltaTime();
		if (shakeTimer_ <= 0.0f) {
			shakeTimer_ = 0.0f;
			return;
		}

		constexpr float kShakeAmplitude = 0.6f;
		const float strength = kShakeAmplitude * (shakeTimer_ / shakeDuration_);
		std::uniform_real_distribution<float> offset(-1.0f, 1.0f);
		transform_.translate.x += offset(shakeRandom_) * strength;
		transform_.translate.y += offset(shakeRandom_) * strength;
	}

	Transform transform_{};
	std::unique_ptr<Camera> camera_;
	Vector3 playerPosition_{};
	Vector3 targetPosition_{};
	Vector3 followPosition_{};

	float orbitYaw_ = 0.0f;
	float orbitPitch_ = 0.55f;
	float distance_ = 15.0f;
	float shakeTimer_ = 0.0f;
	float shakeDuration_ = 0.0f;
	bool hasTarget_ = false;
	bool hasFollowPosition_ = false;
	std::mt19937 shakeRandom_{std::random_device{}()};
};
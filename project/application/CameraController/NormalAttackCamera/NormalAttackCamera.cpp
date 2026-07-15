#include "NormalAttackCamera.h"
#include "Camera.h"
#include "Function.h"
#include "Input.h"
#include <cmath>
#include <imgui.h>
#include <numbers>
#include <random>

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

float GetNearestAngle(float current, float target) { return current + NormalizeAngle(target - current); }

} // namespace

NormalAttackCamera::NormalAttackCamera() {}
NormalAttackCamera::~NormalAttackCamera() {}
void NormalAttackCamera::Initialize() {

	transform_ = {
	    .scale{1.0f,        1.0f,      1.0f  },
        .rotate{orbitPitch_, orbitYaw_, 0.0f  },
        .translate{0.0f,        10.0f,     -50.0f}
    };

	camera_ = std::make_unique<Camera>();
	camera_->SetTransform(transform_);
}
void NormalAttackCamera::Update() {

	if (hasTarget_) {
		Vector3 toTarget = targetPos_ - playerPos;
		if (Function::LengthSquared(toTarget) > 0.0001f) {
			orbitYaw_ = GetNearestAngle(orbitYaw_, std::atan2f(toTarget.x, toTarget.z));
		}
	}

	const float maxPitch = 1.2f;
	const float minPitch = -1.2f;
	if (orbitPitch_ > maxPitch) {
		orbitPitch_ = maxPitch;
	}
	if (orbitPitch_ < minPitch) {
		orbitPitch_ = minPitch;
	}
	if (!hasTarget_ && hasFollowPosition_) {
		Vector3 toFollow = playerPos - followPosition_;
		if (Function::LengthSquared(toFollow) > 0.0001f) {
			orbitYaw_ = GetNearestAngle(orbitYaw_, std::atan2f(toFollow.x, toFollow.z));
		}
	}
	Vector3 orbitDir = {sinf(orbitYaw_) * cosf(orbitPitch_), -sinf(orbitPitch_), cosf(orbitYaw_) * cosf(orbitPitch_)};

	transform_.translate = playerPos - orbitDir * distance_;
	transform_.rotate = {orbitPitch_, orbitYaw_, 0.0f};

	if (shakeTimer_ > 0.0f) {
		shakeTimer_ -= 1.0f / 60.0f;
		if (shakeTimer_ < 0.0f) {
			shakeTimer_ = 0.0f;
		}
		const float decay = shakeDuration_ > 0.0f ? (shakeTimer_ / shakeDuration_) : 0.0f;
		const float strength = shakeAmplitude_ * decay;
		static std::mt19937 rng{std::random_device{}()};
		std::uniform_real_distribution<float> offsetDist(-1.0f, 1.0f);
		transform_.translate.x += offsetDist(rng) * strength;
		transform_.translate.y += offsetDist(rng) * strength;
	}

	camera_->SetTransform(transform_);
	camera_->Update();
}
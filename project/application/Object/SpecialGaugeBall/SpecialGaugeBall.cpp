#include "SpecialGaugeBall.h"
#include "Camera.h"
#include <cmath>
#include <numbers>

namespace {
const Vector4 kExpCubeColor = {0.6f, 1.0f, 0.3f, 1.0f};
const float kRotateSpeed = 0.03f;
const float kReturnSpeed = 0.2f;
const float kAutoCollectRange = 5.0f;
const float kAutoCollectLerpRate = 0.15f;
const float kCollectDistance = 0.15f;
const Vector3 kRotateOffset[3] = {
    {0.0f,                             std::numbers::pi_v<float> / 2.0f, 0.0f                            },
    {std::numbers::pi_v<float> / 2.0f, 1.0f,                             0.0f                            },
    {0.0f,                             1.0f,                             std::numbers::pi_v<float> / 2.0f}
};
} // namespace

bool SpecialGaugeBall::TryAutoCollect(const Vector3& playerPosition) {
	if (isCollected_) {
		return false;
	}

	const float dx = playerPosition.x - baseTransform_.translate.x;
	const float dy = playerPosition.y - baseTransform_.translate.y;
	const float dz = playerPosition.z - baseTransform_.translate.z;
	if (!isAutoCollecting_ && dx * dx + dz * dz > kAutoCollectRange * kAutoCollectRange) {
		return false;
	}

	isAutoCollecting_ = true;
	baseTransform_.translate.x += dx * kAutoCollectLerpRate;
	baseTransform_.translate.y += dy * kAutoCollectLerpRate;
	baseTransform_.translate.z += dz * kAutoCollectLerpRate;

	if (dx * dx + dy * dy + dz * dz > kCollectDistance * kCollectDistance) {
		return false;
	}

	baseTransform_.translate = playerPosition;
	isCollected_ = true;
	return true;
}

void SpecialGaugeBall::Initialize(Camera* camera, const Vector3& position) {
	camera_ = camera;
	isCollected_ = false;
	isAutoCollecting_ = false;
	baseTransform_ = {
	    {0.35f,      0.35f,      0.35f     },
	    {0.0f,       0.0f,       0.0f      },
	    {position.x, position.y, position.z},
	};
	for (int i = 0; i < 3; i++) {
		primitive_[i] = std::make_unique<Primitive>();
		primitive_[i]->Initialize(Primitive::Box, "Resources/2d/defaultParticle.png");
		primitive_[i]->SetEnableLighting(false);
		primitive_[i]->SetColor(kExpCubeColor);
		primitive_[i]->SetTransform(baseTransform_);
	}
}

void SpecialGaugeBall::Update(const Vector3& movementLimitCenter, float movementLimitRadius) {
	if (isCollected_) {
		return;
	}

	const float dx = baseTransform_.translate.x - movementLimitCenter.x;
	const float dz = baseTransform_.translate.z - movementLimitCenter.z;
	const float distSquared = dx * dx + dz * dz;
	if (!isAutoCollecting_ && distSquared > movementLimitRadius * movementLimitRadius) {
		const float dist = std::sqrt(distSquared);
		const float scale = movementLimitRadius / dist;
		const Vector3 target = {
		    movementLimitCenter.x + dx * scale,
		    baseTransform_.translate.y,
		    movementLimitCenter.z + dz * scale,
		};
		const float toX = target.x - baseTransform_.translate.x;
		const float toZ = target.z - baseTransform_.translate.z;
		const float moveDistSquared = toX * toX + toZ * toZ;
		if (moveDistSquared <= kReturnSpeed * kReturnSpeed) {
			baseTransform_.translate.x = target.x;
			baseTransform_.translate.z = target.z;
		} else {
			const float moveDist = std::sqrt(moveDistSquared);
			baseTransform_.translate.x += (toX / moveDist) * kReturnSpeed;
			baseTransform_.translate.z += (toZ / moveDist) * kReturnSpeed;
		}
	}

	baseTransform_.rotate.y += kRotateSpeed;
	for (int i = 0; i < 3; i++) {
		Transform transform = baseTransform_;
		transform.rotate.x += kRotateOffset[i].x;
		transform.rotate.y += kRotateOffset[i].y;
		transform.rotate.z += kRotateOffset[i].z;
		primitive_[i]->SetTransform(transform);
		primitive_[i]->SetCamera(camera_);
		primitive_[i]->Update();
	}

}

void SpecialGaugeBall::Draw() {
	if (isCollected_) {
		return;
	}
	for (int i = 0; i < 3; i++) {
		primitive_[i]->Draw();
	}
}
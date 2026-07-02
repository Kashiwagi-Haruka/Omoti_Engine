#include "SpecialGaugeBallManager.h"
#include "Camera.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <cstdlib>

namespace {
const float kDropSpread = 0.8f;
const float kDropLift = 0.2f;
} // namespace

void SpecialGaugeBallManager::Initialize(Camera* camera) {
	camera_ = camera;
	SpecialGaugeBalls_.clear();
}

void SpecialGaugeBallManager::Update(Camera* camera, const Vector3& movementLimitCenter, float movementLimitRadius) {
	camera_ = camera;
	for (auto& cube : SpecialGaugeBalls_) {
		if (!cube->IsCollected()) {
			cube->SetCamera(camera_);
			cube->Update(movementLimitCenter, movementLimitRadius);
		}
	}
	RemoveCollected();
}

void SpecialGaugeBallManager::Draw() {
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);
	for (auto& cube : SpecialGaugeBalls_) {
		cube->Draw();
	}
	Object3dCommon::GetInstance()->DrawCommon();
}

void SpecialGaugeBallManager::SpawnDrops(const Vector3& position, int count) {
	for (int i = 0; i < count; ++i) {
		float offsetX = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * kDropSpread;
		float offsetZ = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * kDropSpread;
		Vector3 spawnPos = position;
		spawnPos.x += offsetX;
		spawnPos.y += kDropLift;
		spawnPos.z += offsetZ;

		auto cube = std::make_unique<SpecialGaugeBall>();
		cube->Initialize(camera_, spawnPos);
		SpecialGaugeBalls_.push_back(std::move(cube));
	}
}

void SpecialGaugeBallManager::RemoveCollected() {
	SpecialGaugeBalls_.erase(std::remove_if(SpecialGaugeBalls_.begin(), SpecialGaugeBalls_.end(), [](const std::unique_ptr<SpecialGaugeBall>& cube) { return cube->IsCollected(); }), SpecialGaugeBalls_.end());
}
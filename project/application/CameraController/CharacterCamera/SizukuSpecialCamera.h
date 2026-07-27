#pragma once
#include "Camera.h"
#include "Transform.h"
#include <memory>

/// <summary>
/// シズクの必殺技中に、プレイヤーの正面から姿を映すカメラ。
/// </summary>
class SizukuSpecialCamera {
	std::unique_ptr<Camera> camera_;
	Transform transform_{};
	Transform playerTransform_{};

	float distance_ = 6.0f;
	float cameraHeight_ = 2.2f;
	float lookAtHeight_ = 1.8f;

public:
	void Initialize();
	void Update();

	void SetPlayerTransform(const Transform& transform) { playerTransform_ = transform; }
	Camera* GetCamera() const { return camera_.get(); }
	const Transform& GetTransform() const { return transform_; }
};
#pragma once
#include "Camera.h"
#include "Transform.h"
#include <memory>

class SizukuSpecialCamera {

	enum class CameraCut{
		TURN,
		FINGERSNAP,
		ATTACK,
	};

	CameraCut cameraCut_;

	std::unique_ptr<Camera> camera_;
	Transform transform_{};
	Transform playerTransform_{};

	float distance_ = 2.0f;
	float cameraHeight_ = 2.2f;
	float lookAtHeight_ = 1.8f;

	float animationTime_ = 0.0f;

	float TurnAnimationTime_ = 2.0f; /*秒*/ 
	float FingerSnapAnimationTime_ = 1.0f; /*秒*/ 
	float AttackAnimationTime_ = 2.0f;      /*秒*/ 

	bool isEnd_ = false;

public:
	void Initialize();
	void Update();
	void Start();
	bool GetIsEnd() const { return isEnd_; };

	void SetPlayerTransform(const Transform& transform) { playerTransform_ = transform; }
	Camera* GetCamera() const { return camera_.get(); }
	const Transform& GetTransform() const { return transform_; }
};
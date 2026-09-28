#pragma once
#include "Camera.h"
#include "Object/Characters/Base/Attribute.h"
#include "Transform.h"
#include <memory>

class SizukuSpecialCamera {

	enum class CameraCut {
		TURN,
		FINGERSNAP,
		ATTACK,
	};

	CameraCut cameraCut_;

	std::unique_ptr<Camera> camera_;
	Transform transform_{};
	Transform playerTransform_{};

	float distance_ = 2.0f;
	float cameraHeight_ = 1.2f;
	float lookAtHeight_ = 1.2f;

	float animationTime_ = 0.0f;

	float TurnAnimationTime_ = 2.0f;       /*秒*/
	float FingerSnapAnimationTime_ = 1.0f; /*秒*/
	float AttackAnimationTime_ = 2.0f;     /*秒*/

	bool isEnd_ = false;
	Attribute attribute_ = Attribute::Ice;

	void UpdateFireCamera(float deltaTime);
	void UpdateIceCamera(float deltaTime);
	void UpdateWindCamera(float deltaTime);
	void UpdateThunderCamera(float deltaTime);
	void UpdateImaginaryCamera(float deltaTime);
	void UpdateQuantumCamera(float deltaTime);
	void LookAt(const Vector3& target);

public:
	void Initialize();
	void Update();
	void Start();
	bool GetIsEnd() const { return isEnd_; };

	void SetPlayerTransform(const Transform& transform) { playerTransform_ = transform; }
	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Camera* GetCamera() const { return camera_.get(); }
	const Transform& GetTransform() const { return transform_; }
};
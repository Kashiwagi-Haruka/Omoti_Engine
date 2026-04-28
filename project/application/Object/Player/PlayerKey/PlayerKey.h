#pragma once
#include "Mesh/Object3d/Object3d.h"
#include "Mesh/Primitive/Primitive.h"
#include "Transform.h"
#include <memory>
class Camera;
class PlayerKey {

	enum class KeyAnimation {
		Normal,
		Start,
		Open,
	} keyAnimation_ = KeyAnimation::Normal;

	Camera* camera_;

	std::unique_ptr<Object3d> keyObj_;
	std::unique_ptr<Primitive> keyHoleObj_;
	std::unique_ptr<Primitive> planeObj_;

	Transform playerTransform_;
	Transform keyTransform_;
	Transform keyHoleTransform_;
	Transform planeTransform_;

	float planeRotateVelocity_ = 0.0f;

	bool isAninmationStarted_ = false;

	public:
		PlayerKey();
		~PlayerKey();
		void Initialize();
		void Update();
	    void Draw();
	    void SetPlayerTransform(const Transform& transform) { playerTransform_ = transform; }
	    void SetCamera(Camera* camera) { camera_ = camera; }
	    void StartAnimation() { isAninmationStarted_ = true; }
};

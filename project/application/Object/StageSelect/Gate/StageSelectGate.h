#pragma once
#include "Object3d/Object3d.h"
#include "Primitive/Primitive.h"
#include <memory>
#include "Engine/math/RigidBody.h"

class Camera;

class StageSelectGate {

	std::unique_ptr<Object3d> gate_;
	std::unique_ptr<Primitive> Portal_;
	Transform gateTransform_{};
	AABB gateAABB_{};


	Camera* camera_ = nullptr;

	public:
		StageSelectGate();
	    ~StageSelectGate() = default;
	    void Initialize();
	    void Update();
	    void Draw();
	    void SetCamera(Camera* camera) { camera_ = camera; }
};

#pragma once
#include "Primitive/Primitive.h"
#include "Transform.h"
#include "Vector3.h"
#include "Vector4.h"
#include <memory>
#include <array>

class Camera;

class SpecialGaugeBall {
public:
	void Initialize(Camera* camera, const Vector3& position);
	void Update(const Vector3& movementLimitCenter, float movementLimitRadius);
	void Draw();
	void SetCamera(Camera* camera) { camera_ = camera; }
	Vector3 GetPosition() const { return baseTransform_.translate; }
	Vector3 GetScale() const { return baseTransform_.scale; }
	bool IsCollected() const { return isCollected_; }
	void Collect() { isCollected_ = true; }

private:
	std::array<std::unique_ptr<Primitive>, 3> primitive_;
	Transform baseTransform_ = {
	    {1.0f, 1.0f, 1.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	};
	Camera* camera_ = nullptr;
	bool isCollected_ = false;
};
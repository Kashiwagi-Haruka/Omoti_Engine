#pragma once
#include "Primitive/Primitive.h"
#include "Vector3.h"
#include <memory>

class Camera;

class LockOnEnemy {
	std::unique_ptr<Primitive> lockOnPrimitive_;
	Camera* camera_ = nullptr;
	Vector3 position_{};
	float verticalOffset_ = 3.95f;
	float size_ = 0.55f;
	bool isActive_ = false;

	void ApplyBillboardTransform();

public:
	void Initialize();
	void Update();
	void Draw();
	void SetPosition(const Vector3& position);
	void SetCamera(Camera* camera);
	void SetActive(bool isActive);
};
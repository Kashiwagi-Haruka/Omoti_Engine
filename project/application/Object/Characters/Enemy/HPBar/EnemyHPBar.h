#pragma once
#include "Primitive/Primitive.h"
#include "Vector3.h"
#include <memory>
class Camera;
class EnemyHPBar {

	std::unique_ptr<Primitive> hpBarPrimitive_;
	Camera* camera_ = nullptr;
	int HP_ = 1;
	int maxHP_ = 1;
	Vector3 position_{};
	float width_ = 1.8f;
	float height_ = 0.18f;
	float verticalOffset_ = 3.4f;

	void ApplyBillboardTransform();

public:
	void Initialize();
	void Update();
	void Draw();
	void SetHP(int HP);
	void SetMaxHP(int maxHP);
	void SetPosition(const Vector3& position);
	void SetCamera(Camera* camera);
};
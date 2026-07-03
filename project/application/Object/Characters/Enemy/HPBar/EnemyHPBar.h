#pragma once
#include "Primitive/Primitive.h"
#include <memory>
#include "Vector3.h"
class Camera;
class EnemyHPBar {

	std::unique_ptr<Primitive> hpBarPrimitive_;
	Camera* camera_ = nullptr;
	int HP_ = 1;
	int maxHP_ = 1;
	Vector3 position_{};

	public:
	void Initialize();
	void Update();
	void Draw();
	void SetHP(int HP);
	void SetMaxHP(int maxHP);
	void SetPosition(const Vector3& position);
	void SetCamera(Camera* camera);
};

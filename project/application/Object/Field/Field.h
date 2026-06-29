#pragma once
#include "Transform.h"
#include "Vector3.h"
#include <memory>
#include "Primitive/Primitive.h"
class Camera;

class Field {

	std::unique_ptr<Primitive> field_ = nullptr;

	Camera* camera_ = nullptr;
	Transform transform_;
	Vector3 initPos_ = {50, 50, 0};

public:
	Field();
	~Field();

	void Initialize(Camera* camera);
	void Update();
	void Draw();
	void SetCamera(Camera* camera) { camera_ = camera; }
};

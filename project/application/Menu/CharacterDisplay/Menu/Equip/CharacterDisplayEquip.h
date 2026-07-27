#pragma once
#include <array>
#include <memory>
#include "Primitive/Primitive.h"
class Camera;
class CharacterDisplayEquip {

	std::array<std::unique_ptr<Primitive>, 5> viewEquipment_;
	std::array<Transform, 5> viewEquipmentTransform_;
	float distance_ = 2.0f; //中心からの距離
	Vector3 centerTranslate_;
	Camera* camera_;

public:
	void Initialize();
	void Update();
	void Draw();
	void SetCamera(Camera* camera);
	void SetTranslate(Vector3 centerTranslate) { centerTranslate_ = {centerTranslate.x,centerTranslate.y+0.5f,centerTranslate.z}; };
};

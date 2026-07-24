#pragma once
#include <array>
#include <memory>
#include "Primitive/Primitive.h"
class CharacterDisplayEquip {

	std::array<std::unique_ptr<Primitive>, 5> viewEquipment_;
	std::array<std::unique_ptr<Transform>, 5> viewEquipmentTransform_;
	float distance_ = 2.0f; //中心からの距離
	Vector3 centerTranslate_;


public:
	void Initialize();
	void Update();
	void Draw();
	void SetTranslate(Vector3 centerTranslate) { centerTranslate_ = centerTranslate; };
};

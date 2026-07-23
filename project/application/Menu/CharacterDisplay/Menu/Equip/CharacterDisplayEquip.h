#pragma once
#include <array>
#include <memory>
#include "Primitive/Primitive.h"
class CharacterDisplayEquip {
	
	std::array<std::unique_ptr<Primitive>, 5> viewWquipment_;

public:
	void Initialize();
	void Update();
	void Draw();
};

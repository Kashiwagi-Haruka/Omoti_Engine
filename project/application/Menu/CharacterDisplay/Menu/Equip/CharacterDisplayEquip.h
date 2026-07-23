#pragma once
#include "Object/Characters/Playable/Base/Equipment.h"
#include <array>
#include <memory>
class CharacterDisplayEquip {
	
	std::array<std::unique_ptr<BaseEquipment>, 5> viewWquipment_;


public:
	void Initialize();
	void Update();
	void Draw();
};

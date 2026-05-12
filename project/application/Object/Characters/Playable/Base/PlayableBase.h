#pragma once
#include "Object/Characters/Base/Attribute.h"
#include "Object/Characters/Playable/Base/WeaponTypes.h"
#include "Engine/math/Transform.h"
#include "CharacterParameters.h"

class PlayableBase {

protected:
	Transform transform_;
	Attribute attribute_ = Attribute::None;
	WeaponTypes weaponTypes_ = WeaponTypes::NONE;
	BaseParameter baseParameter_;
	Parameter parameter_;
	bool isHaveCharacter_;

public:
	PlayableBase() = default;
	virtual ~PlayableBase() = default;

	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Attribute GetAttribute() const { return attribute_; }
	void SetWeaponTypes(WeaponTypes weaponTypes) { weaponTypes_ = weaponTypes; }
	WeaponTypes GetWeaponTypes() const { return weaponTypes_; }
};

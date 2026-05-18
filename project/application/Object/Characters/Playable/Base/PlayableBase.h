#pragma once
#include "Camera.h"
#include "Engine/math/Transform.h"
#include "Object/Characters/Base/Attribute.h"
#include "Object/Characters/Base/CharacterParameters.h"
#include "Object/Characters/Playable/Base/WeaponTypes.h"
#include "Object3d/Object3d.h"
#include <optional>
#include <string>

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
	virtual void Initialize() = 0;
	virtual void SetAnimation(std::string name) = 0;
	virtual void SetCamera(Camera* camera) = 0;
	virtual void SetTransform(Transform transform) { transform_ = transform; }
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual std::optional<Matrix4x4> GetJointWorldMatrix(const std::string& jointName) const = 0;
	virtual bool IsAnimationFinished() const = 0;
	virtual Object3d* GetObject3d() = 0;

	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Attribute GetAttribute() const { return attribute_; }
	void SetWeaponTypes(WeaponTypes weaponTypes) { weaponTypes_ = weaponTypes; }
	WeaponTypes GetWeaponTypes() const { return weaponTypes_; }
};
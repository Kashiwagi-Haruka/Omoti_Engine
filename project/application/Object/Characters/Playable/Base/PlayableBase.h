#pragma once
#include "Camera.h"
#include "Engine/math/Transform.h"
#include "Object/Characters/Base/Attribute.h"
#include "Object/Characters/Base/CharacterParameters.h"
#include "Object/Characters/Playable/Base/Arcana.h"
#include "Object/Characters/Playable/Base/WeaponTypes.h"
#include "Object3d/Object3d.h"
#include "SkilltreeType.h"
#include <optional>
#include <string>

class PlayableBase {

protected:
	//トランスフォーム
	Transform transform_;
	// 名前
	std::string name_;
	// ローマ字名前
	std::string romanizationName_;
	// 属性
	Attribute attribute_ = Attribute::None;
	// 武器タイプ
	WeaponTypes weaponTypes_ = WeaponTypes::NONE;
	// アルカナ
	Arcana arcana_ = Arcana::FOOL;
	// 基礎パラメータ
	BaseParameter baseParameter_;
	// パラメータ
	Parameter parameter_;
	// スキルツリー
	SkilltreeType skilltree_{};
	// 今の必殺技ゲージ
	int specialGauge_;
	// 最大必殺技ゲージ
	int specialGaugeMax_;

	// 強化量
	int reinforcementAmount_ = 0;
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
	void SetBaseParameter(const BaseParameter& baseParameter) { baseParameter_ = baseParameter; }
	const BaseParameter& GetBaseParameter() const { return baseParameter_; }
	void SetParameter(const Parameter& parameter) { parameter_ = parameter; }
	const Parameter& GetParameter() const { return parameter_; }
	void SetName(const std::string& name) { name_ = name; }
	const std::string& GetName() const { return name_; }
	void SetRomanizationName(const std::string& romanizationName) { romanizationName_ = romanizationName; }
	const std::string& GetRomanizationName() const { return romanizationName_; }
	void SetArcana(Arcana arcana) { arcana_ = arcana; }
	Arcana GetArcana() const { return arcana_; }
	const SkilltreeType& GetSkilltreeType() const { return skilltree_; }
	void SetReinforcementAmount(int reinforcementAmount) { reinforcementAmount_ = reinforcementAmount; }
	const int GetReinforcement() const { return reinforcementAmount_; }
};
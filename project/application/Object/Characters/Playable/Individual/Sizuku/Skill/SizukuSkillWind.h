#pragma once
#include "SizukuSkillAttributeBase.h"
class SizukuSkill;
// シズクの風属性スキルを担当するクラス。
class SizukuSkillWind final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
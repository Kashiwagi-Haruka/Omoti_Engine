#pragma once
#include "SizukuSkillAttributeBase.h"
class SizukuSkill;
// シズクの虚数属性スキルを担当するクラス。
class SizukuSkillImaginary final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
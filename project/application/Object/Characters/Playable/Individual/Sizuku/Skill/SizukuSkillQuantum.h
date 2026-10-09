#pragma once
#include "SizukuSkillAttributeBase.h"
class SizukuSkill;
// シズクの量子属性スキルを担当するクラス。
class SizukuSkillQuantum final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
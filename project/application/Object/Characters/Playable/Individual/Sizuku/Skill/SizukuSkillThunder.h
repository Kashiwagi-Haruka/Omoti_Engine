#pragma once
#include "SizukuSkillAttributeBase.h"

// シズクの雷属性スキルを担当するクラス。
class SizukuSkillThunder final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
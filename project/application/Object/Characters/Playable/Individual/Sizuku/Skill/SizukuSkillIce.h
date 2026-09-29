#pragma once
#include "SizukuSkillAttributeBase.h"

// シズクの氷属性スキルを担当するクラス。
class SizukuSkillIce final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
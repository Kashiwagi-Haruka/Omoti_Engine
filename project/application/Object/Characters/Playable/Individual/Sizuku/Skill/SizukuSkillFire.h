#pragma once
#include "SizukuSkillAttributeBase.h"

class SizukuSkill;

// シズクの火属性スキルを担当するクラス。
class SizukuSkillFire final : public SizukuSkillAttributeBase {
public:
	void Start(SizukuSkill& skill, const Transform& playerTransform) override;
	void Update(SizukuSkill& skill) override;
};
#pragma once

#include "Transform.h"

class SizukuSkill;

// 属性ごとのスキル演出を差し替えるための基底クラス。
class SizukuSkillAttributeBase {
public:
	virtual ~SizukuSkillAttributeBase() = default;
	virtual void Start(SizukuSkill& skill, const Transform& playerTransform) = 0;
	virtual void Update(SizukuSkill& skill) = 0;
};
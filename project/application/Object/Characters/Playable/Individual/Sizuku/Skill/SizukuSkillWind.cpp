#include "SizukuSkillWind.h"
#include "SizukuSkill.h"

void SizukuSkillWind::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {0.25f, 1.0f, 0.55f, 1.0f}, {0.75f, 1.0f, 0.85f, 1.0f}); }

void SizukuSkillWind::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }
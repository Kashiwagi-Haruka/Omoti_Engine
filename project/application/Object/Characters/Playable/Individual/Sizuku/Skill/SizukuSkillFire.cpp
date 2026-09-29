#include "SizukuSkillFire.h"
#include "SizukuSkill.h"

void SizukuSkillFire::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {1.0f, 0.18f, 0.04f, 1.0f}, {1.0f, 0.85f, 0.15f, 1.0f}); }

void SizukuSkillFire::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }
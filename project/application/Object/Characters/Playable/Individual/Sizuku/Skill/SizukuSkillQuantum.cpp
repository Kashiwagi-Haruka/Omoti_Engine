#include "SizukuSkillQuantum.h"
#include "SizukuSkill.h"

void SizukuSkillQuantum::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {0.55f, 0.12f, 1.0f, 1.0f}, {0.15f, 0.65f, 1.0f, 1.0f}); }

void SizukuSkillQuantum::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }
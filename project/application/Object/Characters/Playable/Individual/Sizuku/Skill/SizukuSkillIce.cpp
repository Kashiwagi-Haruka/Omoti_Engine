#include "SizukuSkillIce.h"
#include "SizukuSkill.h"

void SizukuSkillIce::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {0.2f, 0.75f, 1.0f, 1.0f}, {0.75f, 0.95f, 1.0f, 1.0f}); }

void SizukuSkillIce::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }
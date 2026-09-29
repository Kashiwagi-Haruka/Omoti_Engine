#include "SizukuSkillThunder.h"
#include "SizukuSkill.h"

void SizukuSkillThunder::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {0.75f, 0.55f, 1.0f, 1.0f}, {0.95f, 0.85f, 1.0f, 1.0f}); }

void SizukuSkillThunder::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }
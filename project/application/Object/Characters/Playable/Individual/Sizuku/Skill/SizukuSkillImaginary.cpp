#include "SizukuSkillImaginary.h"
#include "SizukuSkill.h"

void SizukuSkillImaginary::Start(SizukuSkill& skill, const Transform& playerTransform) { skill.StartAttributeSkill(playerTransform, {1.0f, 0.78f, 0.18f, 1.0f}, {1.0f, 0.95f, 0.65f, 1.0f}); }

void SizukuSkillImaginary::Update(SizukuSkill& skill) { skill.UpdateAttributeSkill(); }